/**
 * CYBERFLIPPER — Code Repeater
 * Captures Sub-GHz signals and replays them with configurable delay,
 * repeat count, and frequency offset. Supports raw capture mode.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <subghz/devices/devices.h>
#include <furi_hal_region.h>
#include <stdlib.h>
#include <string.h>

#define TAG "CodeRepeater"
#define CAPTURE_BUFFER_SIZE 4096
#define MAX_CAPTURES 16

static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 3,
    .bands = {
        {.start = 299999755, .end = 348000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 386999938, .end = 464000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 778999847, .end = 928000000, .power_limit = 20, .duty_cycle = 50},
    },
};

typedef enum {
    RepeaterModeCapture,
    RepeaterModeReplay,
    RepeaterModeSettings,
} RepeaterMode;

typedef struct {
    uint8_t data[CAPTURE_BUFFER_SIZE];
    uint32_t length;
    uint32_t frequency;
    uint32_t timestamp;
    bool valid;
} CapturedSignal;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    const SubGhzDevice* device;
    SubGhzTxRxWorker* subghz_txrx;
    FuriThread* worker_thread;
    
    bool running;
    bool worker_running;
    RepeaterMode mode;
    
    // Capture storage
    CapturedSignal captures[MAX_CAPTURES];
    uint8_t capture_count;
    uint8_t selected_capture;
    
    // Live capture buffer
    uint8_t rx_buffer[CAPTURE_BUFFER_SIZE];
    uint32_t rx_len;
    bool capturing;
    
    // Replay settings
    uint32_t frequency;
    uint32_t replay_delay_ms;
    uint32_t replay_count;
    uint32_t replays_done;
    bool replaying;
    
    // Frequency scanning
    uint8_t freq_preset;
} CodeRepeaterApp;

static const uint32_t common_frequencies[] = {
    300000000, 303875000, 310000000, 315000000,
    318000000, 390000000, 418000000, 433075000,
    433420000, 433920000, 434420000, 434775000,
    438900000, 868350000, 915000000, 925000000,
};
#define NUM_COMMON_FREQS (sizeof(common_frequencies) / sizeof(common_frequencies[0]))

// --- Capture Thread ---

static int32_t repeater_capture_thread(void* context) {
    CodeRepeaterApp* app = context;
    FURI_LOG_I(TAG, "Capture thread started at %lu Hz", app->frequency);
    
    while(app->worker_running) {
        if(app->capturing && app->subghz_txrx) {
            size_t available = subghz_tx_rx_worker_available(app->subghz_txrx);
            if(available > 0) {
                uint32_t to_read = available;
                if(app->rx_len + to_read > CAPTURE_BUFFER_SIZE) {
                    to_read = CAPTURE_BUFFER_SIZE - app->rx_len;
                }
                if(to_read > 0) {
                    subghz_tx_rx_worker_read(
                        app->subghz_txrx, 
                        &app->rx_buffer[app->rx_len], 
                        to_read
                    );
                    app->rx_len += to_read;
                }
                
                // Auto-save when buffer is full
                if(app->rx_len >= CAPTURE_BUFFER_SIZE) {
                    if(app->capture_count < MAX_CAPTURES) {
                        CapturedSignal* sig = &app->captures[app->capture_count];
                        memcpy(sig->data, app->rx_buffer, app->rx_len);
                        sig->length = app->rx_len;
                        sig->frequency = app->frequency;
                        sig->timestamp = furi_get_tick();
                        sig->valid = true;
                        app->capture_count++;
                        FURI_LOG_I(TAG, "Auto-saved capture %d (%lu bytes)", 
                                   app->capture_count, app->rx_len);
                    }
                    app->rx_len = 0;
                    app->capturing = false;
                }
            }
        } else if(app->replaying && app->subghz_txrx) {
            CapturedSignal* sig = &app->captures[app->selected_capture];
            if(sig->valid) {
                for(uint32_t i = 0; i < app->replay_count && app->worker_running; i++) {
                    subghz_tx_rx_worker_write(
                        app->subghz_txrx, sig->data, sig->length
                    );
                    app->replays_done = i + 1;
                    furi_delay_ms(app->replay_delay_ms);
                }
            }
            app->replaying = false;
        }
        furi_delay_ms(10);
    }
    
    return 0;
}

// --- UI ---

static void repeater_draw_callback(Canvas* canvas, void* context) {
    CodeRepeaterApp* app = (CodeRepeaterApp*)context;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, "CYBER CODE REPEATER");
    canvas_draw_line(canvas, 0, 12, 128, 12);
    
    canvas_set_font(canvas, FontSecondary);
    
    char freq_str[32];
    snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%03lu MHz", 
             app->frequency / 1000000,
             (app->frequency % 1000000) / 1000);
    canvas_draw_str(canvas, 2, 23, freq_str);
    
    switch(app->mode) {
        case RepeaterModeCapture: {
            if(app->capturing) {
                char cap_str[32];
                snprintf(cap_str, sizeof(cap_str), "RX: %lu bytes", app->rx_len);
                canvas_draw_str(canvas, 2, 33, cap_str);
                canvas_draw_str(canvas, 2, 43, ">> CAPTURING... <<");
                canvas_draw_str(canvas, 2, 55, "[OK] Save  [BACK] Stop");
            } else {
                char count_str[32];
                snprintf(count_str, sizeof(count_str), "Captures: %d/%d", 
                         app->capture_count, MAX_CAPTURES);
                canvas_draw_str(canvas, 2, 33, count_str);
                canvas_draw_str(canvas, 2, 43, "[OK] Capture [>] Replay");
                canvas_draw_str(canvas, 2, 55, "[UP/DN] Freq [BACK] Exit");
            }
            break;
        }
        case RepeaterModeReplay: {
            char sel_str[32];
            snprintf(sel_str, sizeof(sel_str), "Signal: %d/%d", 
                     app->selected_capture + 1, app->capture_count);
            canvas_draw_str(canvas, 2, 33, sel_str);
            
            if(app->replaying) {
                char rep_str[32];
                snprintf(rep_str, sizeof(rep_str), "Replaying: %lu/%lu", 
                         app->replays_done, app->replay_count);
                canvas_draw_str(canvas, 2, 43, rep_str);
            } else {
                char set_str[48];
                snprintf(set_str, sizeof(set_str), "Delay:%lums x%lu", 
                         app->replay_delay_ms, app->replay_count);
                canvas_draw_str(canvas, 2, 43, set_str);
                canvas_draw_str(canvas, 2, 55, "[OK] TX  [<>] Select");
            }
            break;
        }
        case RepeaterModeSettings: {
            char delay_str[32];
            snprintf(delay_str, sizeof(delay_str), "Delay: %lu ms", app->replay_delay_ms);
            canvas_draw_str(canvas, 2, 33, delay_str);
            
            char count_str[32];
            snprintf(count_str, sizeof(count_str), "Repeats: %lu", app->replay_count);
            canvas_draw_str(canvas, 2, 43, count_str);
            canvas_draw_str(canvas, 2, 55, "[UP/DN] Adjust [BACK] Back");
            break;
        }
    }
}

static void repeater_input_callback(InputEvent* input_event, void* context) {
    CodeRepeaterApp* app = (CodeRepeaterApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

// --- App ---

int32_t code_repeater_app(void* p) {
    UNUSED(p);
    
    CodeRepeaterApp* app = malloc(sizeof(CodeRepeaterApp));
    memset(app, 0, sizeof(CodeRepeaterApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->mode = RepeaterModeCapture;
    app->frequency = 433920000;
    app->replay_delay_ms = 500;
    app->replay_count = 3;
    app->freq_preset = 9; // 433.92 MHz index
    app->gui = furi_record_open(RECORD_GUI);
    
    furi_hal_region_set(&unlockedRegion);
    
    view_port_draw_callback_set(app->view_port, repeater_draw_callback, app);
    view_port_input_callback_set(app->view_port, repeater_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    subghz_devices_init();
    app->subghz_txrx = subghz_tx_rx_worker_alloc();
    
    app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeExternalCC1101);
    if(!app->device) {
        app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeInternal);
    }
    
    if(app->device) {
        subghz_devices_reset(app->device);
        subghz_devices_idle(app->device);
        subghz_devices_load_preset(app->device, FuriHalSubGhzPresetOok650Async, NULL);
        subghz_tx_rx_worker_start(app->subghz_txrx, app->device, app->frequency);
        
        app->worker_running = true;
        app->worker_thread = furi_thread_alloc();
        furi_thread_set_name(app->worker_thread, "Repeater Worker");
        furi_thread_set_stack_size(app->worker_thread, 4096);
        furi_thread_set_context(app->worker_thread, app);
        furi_thread_set_callback(app->worker_thread, repeater_capture_thread);
        furi_thread_start(app->worker_thread);
    }
    
    furi_hal_power_suppress_charge_enter();
    
    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 10) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(app->mode) {
                    case RepeaterModeCapture:
                        switch(event.key) {
                            case InputKeyOk:
                                if(app->capturing) {
                                    // Save current capture
                                    if(app->rx_len > 0 && app->capture_count < MAX_CAPTURES) {
                                        CapturedSignal* sig = &app->captures[app->capture_count];
                                        memcpy(sig->data, app->rx_buffer, app->rx_len);
                                        sig->length = app->rx_len;
                                        sig->frequency = app->frequency;
                                        sig->timestamp = furi_get_tick();
                                        sig->valid = true;
                                        app->capture_count++;
                                    }
                                    app->capturing = false;
                                    app->rx_len = 0;
                                } else {
                                    app->rx_len = 0;
                                    app->capturing = true;
                                }
                                break;
                            case InputKeyRight:
                                if(app->capture_count > 0) {
                                    app->mode = RepeaterModeReplay;
                                    app->selected_capture = 0;
                                }
                                break;
                            case InputKeyUp:
                                app->freq_preset = (app->freq_preset + 1) % NUM_COMMON_FREQS;
                                app->frequency = common_frequencies[app->freq_preset];
                                if(app->subghz_txrx && subghz_tx_rx_worker_is_running(app->subghz_txrx)) {
                                    subghz_tx_rx_worker_stop(app->subghz_txrx);
                                    subghz_tx_rx_worker_start(app->subghz_txrx, app->device, app->frequency);
                                }
                                break;
                            case InputKeyDown:
                                app->freq_preset = (app->freq_preset == 0) ? NUM_COMMON_FREQS - 1 : app->freq_preset - 1;
                                app->frequency = common_frequencies[app->freq_preset];
                                if(app->subghz_txrx && subghz_tx_rx_worker_is_running(app->subghz_txrx)) {
                                    subghz_tx_rx_worker_stop(app->subghz_txrx);
                                    subghz_tx_rx_worker_start(app->subghz_txrx, app->device, app->frequency);
                                }
                                break;
                            case InputKeyBack:
                                if(app->capturing) {
                                    app->capturing = false;
                                    app->rx_len = 0;
                                } else {
                                    app->running = false;
                                }
                                break;
                            default: break;
                        }
                        break;
                        
                    case RepeaterModeReplay:
                        switch(event.key) {
                            case InputKeyOk:
                                if(!app->replaying) {
                                    // Restart TX on captured frequency
                                    CapturedSignal* sig = &app->captures[app->selected_capture];
                                    if(app->subghz_txrx && subghz_tx_rx_worker_is_running(app->subghz_txrx)) {
                                        subghz_tx_rx_worker_stop(app->subghz_txrx);
                                    }
                                    subghz_tx_rx_worker_start(app->subghz_txrx, app->device, sig->frequency);
                                    app->replays_done = 0;
                                    app->replaying = true;
                                }
                                break;
                            case InputKeyRight:
                                if(app->selected_capture < app->capture_count - 1)
                                    app->selected_capture++;
                                break;
                            case InputKeyLeft:
                                if(app->selected_capture > 0)
                                    app->selected_capture--;
                                break;
                            case InputKeyUp:
                                app->replay_count++;
                                if(app->replay_count > 100) app->replay_count = 100;
                                break;
                            case InputKeyDown:
                                if(app->replay_count > 1) app->replay_count--;
                                break;
                            case InputKeyBack:
                                app->mode = RepeaterModeCapture;
                                break;
                            default: break;
                        }
                        break;
                        
                    case RepeaterModeSettings:
                        switch(event.key) {
                            case InputKeyUp:
                                app->replay_delay_ms += 100;
                                if(app->replay_delay_ms > 10000) app->replay_delay_ms = 10000;
                                break;
                            case InputKeyDown:
                                if(app->replay_delay_ms > 100) app->replay_delay_ms -= 100;
                                break;
                            case InputKeyBack:
                                app->mode = RepeaterModeReplay;
                                break;
                            default: break;
                        }
                        break;
                }
            }
        }
        view_port_update(app->view_port);
    }
    
    // Cleanup
    app->worker_running = false;
    if(app->worker_thread) {
        furi_thread_join(app->worker_thread);
        furi_thread_free(app->worker_thread);
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
    
    return 0;
}
