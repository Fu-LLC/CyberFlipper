/**
 * CYBERFLIPPER — De Bruijn Sequence Generator
 * Generates and transmits optimal binary De Bruijn sequences B(2,n)
 * for brute-forcing fixed-code systems. Configurable bit length (2-16),
 * frequency, modulation, and timing parameters.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <subghz/devices/devices.h>
#include <furi_hal_region.h>
#include <stdlib.h>
#include <string.h>

#define TAG "DeBruijnGen"
#define MAX_SEQ_LEN 65536

static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 3,
    .bands = {
        {.start = 299999755, .end = 348000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 386999938, .end = 464000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 778999847, .end = 928000000, .power_limit = 20, .duty_cycle = 50},
    },
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    const SubGhzDevice* device;
    SubGhzTxRxWorker* subghz_txrx;
    FuriThread* tx_thread;
    
    bool running;
    bool tx_running;
    bool generating;
    bool transmitting;
    
    uint8_t bit_length;
    uint32_t frequency;
    uint32_t bit_duration_us;
    uint32_t total_codes;
    uint32_t seq_length;
    float progress;
    
    uint8_t cursor;
} DeBruijnApp;

static uint8_t db_a_buf[32];
static uint8_t* db_seq_buf = NULL;
static uint32_t db_seq_pos;

static void db_gen(int t, int p, int n) {
    if(t > n) {
        if(n % p == 0) {
            for(int j = 1; j <= p; j++) {
                if(db_seq_pos < MAX_SEQ_LEN) {
                    db_seq_buf[db_seq_pos++] = db_a_buf[j];
                }
            }
        }
    } else {
        db_a_buf[t] = db_a_buf[t - p];
        db_gen(t + 1, p, n);
        for(int j = db_a_buf[t - p] + 1; j < 2; j++) {
            db_a_buf[t] = j;
            db_gen(t + 1, t, n);
        }
    }
}

static int32_t debruijn_tx_thread(void* context) {
    DeBruijnApp* app = context;
    
    FURI_LOG_I(TAG, "TX: %lu bits at %lu Hz, bit_dur=%lu us",
               app->seq_length, app->frequency, app->bit_duration_us);
    
    // Convert bit sequence to byte packets for transmission
    uint8_t tx_buf[256];
    uint32_t chunk_bits = 256 * 8;
    
    for(uint32_t offset = 0; offset < app->seq_length && app->tx_running;) {
        uint32_t remaining = app->seq_length - offset;
        uint32_t this_chunk = remaining < chunk_bits ? remaining : chunk_bits;
        
        memset(tx_buf, 0, sizeof(tx_buf));
        for(uint32_t i = 0; i < this_chunk; i++) {
            if(db_seq_buf[offset + i]) {
                tx_buf[i / 8] |= (1 << (7 - (i % 8)));
            }
        }
        
        uint32_t tx_bytes = (this_chunk + 7) / 8;
        if(app->subghz_txrx) {
            subghz_tx_rx_worker_write(app->subghz_txrx, tx_buf, tx_bytes);
        }
        
        offset += this_chunk;
        app->progress = (float)offset / (float)app->seq_length * 100.0f;
        
        // Timing based on bit duration
        uint32_t delay = (app->bit_duration_us * this_chunk) / 1000;
        if(delay > 0) furi_delay_ms(delay);
    }
    
    app->transmitting = false;
    FURI_LOG_I(TAG, "TX complete");
    return 0;
}

static void debruijn_draw_callback(Canvas* canvas, void* context) {
    DeBruijnApp* app = (DeBruijnApp*)context;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBER DEBRUIJN GEN");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    
    canvas_set_font(canvas, FontSecondary);
    
    char bits_str[32];
    snprintf(bits_str, sizeof(bits_str), "Bits: %d  Codes: %lu", 
             app->bit_length, app->total_codes);
    canvas_draw_str(canvas, 2, 21, bits_str);
    
    char freq_str[32];
    snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%03lu MHz",
             app->frequency / 1000000,
             (app->frequency % 1000000) / 1000);
    canvas_draw_str(canvas, 2, 31, freq_str);
    
    char dur_str[32];
    snprintf(dur_str, sizeof(dur_str), "Bit dur: %lu us  Seq: %lu",
             app->bit_duration_us, app->seq_length);
    canvas_draw_str(canvas, 2, 41, dur_str);
    
    if(app->transmitting) {
        char prog_str[24];
        snprintf(prog_str, sizeof(prog_str), "TX: %.1f%%", app->progress);
        canvas_draw_str(canvas, 2, 51, prog_str);
        
        canvas_draw_frame(canvas, 2, 53, 124, 6);
        uint32_t fill = (uint32_t)(app->progress * 122.0f / 100.0f);
        canvas_draw_box(canvas, 3, 54, fill, 4);
    } else {
        const char* items[] = {"Bits", "Freq", "Duration"};
        char sel_str[32];
        snprintf(sel_str, sizeof(sel_str), "[UP/DN] %s", items[app->cursor]);
        canvas_draw_str(canvas, 2, 51, sel_str);
        canvas_draw_str(canvas, 2, 61, "[OK]TX [<>]Sel [BACK]Exit");
    }
}

static void debruijn_input_callback(InputEvent* input_event, void* context) {
    DeBruijnApp* app = (DeBruijnApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

int32_t debruijn_gen_app(void* p) {
    UNUSED(p);
    
    DeBruijnApp* app = malloc(sizeof(DeBruijnApp));
    memset(app, 0, sizeof(DeBruijnApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->bit_length = 10;
    app->frequency = 315000000;
    app->bit_duration_us = 500;
    app->total_codes = 1 << app->bit_length;
    app->gui = furi_record_open(RECORD_GUI);
    
    db_seq_buf = malloc(MAX_SEQ_LEN);
    
    furi_hal_region_set(&unlockedRegion);
    
    view_port_draw_callback_set(app->view_port, debruijn_draw_callback, app);
    view_port_input_callback_set(app->view_port, debruijn_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    subghz_devices_init();
    app->subghz_txrx = subghz_tx_rx_worker_alloc();
    furi_hal_power_suppress_charge_enter();
    
    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort && !app->transmitting) {
                switch(event.key) {
                    case InputKeyOk: {
                        // Generate and transmit
                        app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeExternalCC1101);
                        if(!app->device)
                            app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeInternal);
                        
                        if(app->device) {
                            // Generate sequence
                            memset(db_a_buf, 0, sizeof(db_a_buf));
                            db_seq_pos = 0;
                            db_gen(1, 1, app->bit_length);
                            app->seq_length = db_seq_pos;
                            
                            subghz_devices_reset(app->device);
                            subghz_devices_idle(app->device);
                            subghz_devices_load_preset(app->device, FuriHalSubGhzPresetOok650Async, NULL);
                            
                            if(subghz_tx_rx_worker_start(app->subghz_txrx, app->device, app->frequency)) {
                                app->transmitting = true;
                                app->tx_running = true;
                                app->progress = 0;
                                
                                app->tx_thread = furi_thread_alloc();
                                furi_thread_set_name(app->tx_thread, "DeBruijn TX");
                                furi_thread_set_stack_size(app->tx_thread, 4096);
                                furi_thread_set_context(app->tx_thread, app);
                                furi_thread_set_callback(app->tx_thread, debruijn_tx_thread);
                                furi_thread_start(app->tx_thread);
                            }
                        }
                        break;
                    }
                    case InputKeyRight:
                        app->cursor = (app->cursor + 1) % 3;
                        break;
                    case InputKeyLeft:
                        app->cursor = (app->cursor == 0) ? 2 : app->cursor - 1;
                        break;
                    case InputKeyUp:
                        switch(app->cursor) {
                            case 0:
                                if(app->bit_length < 16) app->bit_length++;
                                app->total_codes = 1 << app->bit_length;
                                break;
                            case 1:
                                app->frequency += 1000000;
                                if(app->frequency > 928000000) app->frequency = 300000000;
                                break;
                            case 2:
                                app->bit_duration_us += 50;
                                if(app->bit_duration_us > 5000) app->bit_duration_us = 5000;
                                break;
                        }
                        break;
                    case InputKeyDown:
                        switch(app->cursor) {
                            case 0:
                                if(app->bit_length > 2) app->bit_length--;
                                app->total_codes = 1 << app->bit_length;
                                break;
                            case 1:
                                if(app->frequency > 301000000) app->frequency -= 1000000;
                                break;
                            case 2:
                                if(app->bit_duration_us > 100) app->bit_duration_us -= 50;
                                break;
                        }
                        break;
                    case InputKeyBack:
                        app->running = false;
                        break;
                    default: break;
                }
            } else if(event.type == InputTypeShort && event.key == InputKeyBack && app->transmitting) {
                app->tx_running = false;
            }
        }
        view_port_update(app->view_port);
    }
    
    app->tx_running = false;
    if(app->tx_thread) {
        furi_thread_join(app->tx_thread);
        furi_thread_free(app->tx_thread);
    }
    if(app->subghz_txrx) {
        if(subghz_tx_rx_worker_is_running(app->subghz_txrx))
            subghz_tx_rx_worker_stop(app->subghz_txrx);
        subghz_tx_rx_worker_free(app->subghz_txrx);
    }
    if(app->device) radio_device_loader_end(app->device);
    subghz_devices_deinit();
    furi_hal_power_suppress_charge_exit();
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    if(db_seq_buf) { free(db_seq_buf); db_seq_buf = NULL; }
    free(app);
    
    return 0;
}
