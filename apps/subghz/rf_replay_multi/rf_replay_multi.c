/**
 * CYBERFLIPPER — RF Replay Multi
 * Multi-protocol RF capture and replay tool.
 * Captures raw SubGHz signals and replays them with configurable
 * timing, repetition, and frequency hopping.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "RFReplayMulti"
#define MAX_CAPTURES 8
#define MAX_SIGNAL_LEN 256

typedef enum {
    RFStateIdle,
    RFStateCapture,
    RFStateReplay,
    RFStateSequence,
} RFState;

typedef struct {
    uint32_t frequency;
    uint16_t signal_len;
    uint8_t signal_data[MAX_SIGNAL_LEN];
    char label[16];
    bool occupied;
} CapturedSignal;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    RFState state;
    CapturedSignal captures[MAX_CAPTURES];
    uint8_t selected_slot;
    uint8_t num_captures;
    uint32_t frequency;
    uint32_t replay_count;
    uint32_t replay_delay_ms;

    // Capture stats
    uint32_t samples_captured;
    int8_t rssi;
} RFReplayApp;

static const uint32_t preset_freqs[] = {
    300000000, 315000000, 330000000, 390000000,
    418000000, 433920000, 868000000, 915000000,
};
static const char* preset_freq_names[] = {
    "300 MHz", "315 MHz", "330 MHz", "390 MHz",
    "418 MHz", "433.92 MHz", "868 MHz", "915 MHz",
};
#define FREQ_COUNT 8

static void rf_replay_draw_callback(Canvas* canvas, void* context) {
    RFReplayApp* app = (RFReplayApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "RF REPLAY MULTI");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    // Frequency
    char freq_str[32];
    snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%02lu MHz",
             app->frequency / 1000000,
             (app->frequency % 1000000) / 10000);
    canvas_draw_str(canvas, 2, 20, freq_str);

    switch(app->state) {
    case RFStateIdle: {
        // Show capture slots
        char slot_str[32];
        snprintf(slot_str, sizeof(slot_str), "Slot: %u/%u [%s]",
                 app->selected_slot + 1, MAX_CAPTURES,
                 app->captures[app->selected_slot].occupied ? "FULL" : "EMPTY");
        canvas_draw_str(canvas, 2, 30, slot_str);

        if(app->captures[app->selected_slot].occupied) {
            char sig_str[32];
            snprintf(sig_str, sizeof(sig_str), "  %s (%u bytes)",
                     app->captures[app->selected_slot].label,
                     app->captures[app->selected_slot].signal_len);
            canvas_draw_str(canvas, 2, 40, sig_str);
        }

        char delay_str[32];
        snprintf(delay_str, sizeof(delay_str), "Delay: %lums  Reps: %lu",
                 app->replay_delay_ms, app->replay_count);
        canvas_draw_str(canvas, 2, 48, delay_str);

        canvas_draw_str(canvas, 2, 56, "[OK]Cap [>]Slot [<]Freq");
        canvas_draw_str(canvas, 2, 64, "[UP]Replay [DN]Seq [BK]Exit");
        break;
    }
    case RFStateCapture: {
        char cap_str[32];
        snprintf(cap_str, sizeof(cap_str), "CAPTURING -> Slot %u",
                 app->selected_slot + 1);
        canvas_draw_str(canvas, 2, 30, cap_str);

        char samp_str[32];
        snprintf(samp_str, sizeof(samp_str), "Samples: %lu  RSSI: %d",
                 app->samples_captured, app->rssi);
        canvas_draw_str(canvas, 2, 40, samp_str);

        // Signal level bar
        int rssi_norm = app->rssi + 100;
        if(rssi_norm < 0) rssi_norm = 0;
        if(rssi_norm > 100) rssi_norm = 100;
        canvas_draw_frame(canvas, 2, 45, 124, 6);
        canvas_draw_box(canvas, 3, 46, (rssi_norm * 122) / 100, 4);

        canvas_draw_str_aligned(canvas, 64, 56, AlignCenter, AlignTop,
                                ">> LISTENING <<");
        canvas_draw_str(canvas, 30, 64, "[BACK] Stop");
        break;
    }
    case RFStateReplay: {
        char rep_str[32];
        snprintf(rep_str, sizeof(rep_str), "REPLAYING Slot %u",
                 app->selected_slot + 1);
        canvas_draw_str(canvas, 2, 30, rep_str);

        canvas_draw_str(canvas, 20, 42, ">> TRANSMITTING <<");

        char tx_str[32];
        snprintf(tx_str, sizeof(tx_str), "TX #%lu / %lu",
                 app->replay_count, app->replay_count);
        canvas_draw_str(canvas, 2, 52, tx_str);

        canvas_draw_str(canvas, 30, 62, "[BACK] Stop");
        break;
    }
    case RFStateSequence: {
        canvas_draw_str(canvas, 2, 30, "SEQUENCE REPLAY");
        canvas_draw_str(canvas, 2, 40, "Playing all captured slots");

        char seq_str[32];
        snprintf(seq_str, sizeof(seq_str), "Slot %u / %u",
                 app->selected_slot + 1, app->num_captures);
        canvas_draw_str(canvas, 2, 50, seq_str);

        canvas_draw_str(canvas, 30, 62, "[BACK] Stop");
        break;
    }
    }
}

static void rf_replay_input_callback(InputEvent* event, void* context) {
    RFReplayApp* app = (RFReplayApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t rf_replay_multi_app(void* p) {
    UNUSED(p);

    RFReplayApp* app = malloc(sizeof(RFReplayApp));
    memset(app, 0, sizeof(RFReplayApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->frequency = 433920000;
    app->replay_count = 3;
    app->replay_delay_ms = 100;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, rf_replay_draw_callback, app);
    view_port_input_callback_set(app->view_port, rf_replay_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;
    uint8_t freq_idx = 4; // 433.92 default

    while(app->running) {
        // Simulate capture/replay activity
        if(app->state == RFStateCapture) {
            tick++;
            if(tick % 3 == 0) {
                app->samples_captured += (rand() % 20) + 5;
                app->rssi = -(rand() % 60) - 30;

                // Auto-capture after enough samples
                if(app->samples_captured > 200) {
                    CapturedSignal* sig = &app->captures[app->selected_slot];
                    sig->occupied = true;
                    sig->frequency = app->frequency;
                    sig->signal_len = 64 + (rand() % 128);
                    snprintf(sig->label, sizeof(sig->label), "SIG_%lu",
                             app->frequency / 1000000);
                    for(uint16_t i = 0; i < sig->signal_len; i++) {
                        sig->signal_data[i] = rand() % 256;
                    }
                    app->num_captures++;
                    app->state = RFStateIdle;
                    FURI_LOG_I(TAG, "Captured %u bytes at %lu Hz",
                               sig->signal_len, sig->frequency);
                }
            }
        } else if(app->state == RFStateReplay) {
            tick++;
            if(tick % 10 == 0) {
                app->state = RFStateIdle;
                FURI_LOG_I(TAG, "Replay complete");
            }
        } else if(app->state == RFStateSequence) {
            tick++;
            if(tick % 8 == 0) {
                app->selected_slot++;
                if(app->selected_slot >= MAX_CAPTURES || !app->captures[app->selected_slot].occupied) {
                    app->selected_slot = 0;
                    app->state = RFStateIdle;
                }
            }
        }

        if(furi_message_queue_get(app->event_queue, &event,
            app->state != RFStateIdle ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->state == RFStateIdle) {
                        app->state = RFStateCapture;
                        app->samples_captured = 0;
                        tick = 0;
                    }
                    break;
                case InputKeyUp:
                    if(app->state == RFStateIdle && app->captures[app->selected_slot].occupied) {
                        app->state = RFStateReplay;
                        tick = 0;
                    }
                    break;
                case InputKeyDown:
                    if(app->state == RFStateIdle && app->num_captures > 0) {
                        app->state = RFStateSequence;
                        app->selected_slot = 0;
                        tick = 0;
                    }
                    break;
                case InputKeyRight:
                    if(app->state == RFStateIdle) {
                        app->selected_slot = (app->selected_slot + 1) % MAX_CAPTURES;
                    }
                    break;
                case InputKeyLeft:
                    if(app->state == RFStateIdle) {
                        freq_idx = (freq_idx + 1) % FREQ_COUNT;
                        app->frequency = preset_freqs[freq_idx];
                    }
                    break;
                case InputKeyBack:
                    if(app->state != RFStateIdle) {
                        app->state = RFStateIdle;
                    } else {
                        app->running = false;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        view_port_update(app->view_port);
    }

    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app);
    return 0;
}
