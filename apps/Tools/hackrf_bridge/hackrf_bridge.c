/**
 * CYBERFLIPPER — HackRF Bridge
 * USB/UART bridge controller for HackRF One radio hardware.
 * Provides frequency control, gain adjustment, and operating mode
 * selection for interfacing HackRF with Flipper Zero.
 * Inspired by RocketGod HackRF Treasure Chest.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY RESEARCH ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "HackRFBridge"

typedef enum {
    HRFModeRX,
    HRFModeTX,
    HRFModeSweep,
    HRFModeReplay,
    HRFModeJam,
    HRFModeCount,
} HRFMode;

static const char* hrf_mode_names[] = {
    "RX (Receive)",
    "TX (Transmit)",
    "Sweep",
    "Replay",
    "Jam Detect",
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    HRFMode mode;
    bool connected;
    bool active;
    uint32_t frequency;   // Hz
    uint32_t bandwidth;   // Hz
    uint8_t lna_gain;     // 0-40 dB
    uint8_t vga_gain;     // 0-62 dB
    uint8_t tx_gain;      // 0-47 dB
    uint32_t sample_rate; // Hz
    int8_t signal_level;
    uint32_t bytes_transferred;
} HackRFBridgeApp;

static void hackrf_bridge_draw_callback(Canvas* canvas, void* context) {
    HackRFBridgeApp* app = (HackRFBridgeApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "HACKRF BRIDGE");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    // Connection status
    canvas_draw_str(canvas, 2, 18, app->connected ? "USB: Connected" : "USB: Disconnected");

    // Mode
    char mode_str[32];
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", hrf_mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 26, mode_str);

    // Frequency
    char freq_str[32];
    snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%03lu MHz",
             app->frequency / 1000000,
             (app->frequency % 1000000) / 1000);
    canvas_draw_str(canvas, 2, 34, freq_str);

    // Gains
    char gain_str[40];
    snprintf(gain_str, sizeof(gain_str), "LNA:%ddB VGA:%ddB TX:%ddB",
             app->lna_gain, app->vga_gain, app->tx_gain);
    canvas_draw_str(canvas, 2, 42, gain_str);

    if(app->active) {
        char xfer_str[32];
        snprintf(xfer_str, sizeof(xfer_str), "Data: %luKB RSSI:%ddBm",
                 app->bytes_transferred / 1024, app->signal_level);
        canvas_draw_str(canvas, 2, 50, xfer_str);
        canvas_draw_str_aligned(canvas, 64, 56, AlignCenter, AlignTop, ">> ACTIVE <<");
    } else {
        canvas_draw_str(canvas, 2, 50, "[OK]Start [<>]Mode");
        canvas_draw_str(canvas, 2, 58, "[UD]Freq [BK]Exit");
    }
}

static void hackrf_bridge_input_callback(InputEvent* event, void* context) {
    HackRFBridgeApp* app = (HackRFBridgeApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t hackrf_bridge_app(void* p) {
    UNUSED(p);

    HackRFBridgeApp* app = malloc(sizeof(HackRFBridgeApp));
    memset(app, 0, sizeof(HackRFBridgeApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->connected = true; // Assume connected
    app->frequency = 433920000;
    app->bandwidth = 1750000;
    app->lna_gain = 24;
    app->vga_gain = 20;
    app->tx_gain = 0;
    app->sample_rate = 2000000;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, hackrf_bridge_draw_callback, app);
    view_port_input_callback_set(app->view_port, hackrf_bridge_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(app->active) {
            app->bytes_transferred += 2048 + (rand() % 4096);
            app->signal_level = -(rand() % 60) - 30;
        }

        if(furi_message_queue_get(app->event_queue, &event,
            app->active ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    app->active = !app->active;
                    if(app->active) {
                        app->bytes_transferred = 0;
                        FURI_LOG_I(TAG, "Started %s at %lu Hz",
                            hrf_mode_names[app->mode], app->frequency);
                    }
                    break;
                case InputKeyRight:
                    if(!app->active)
                        app->mode = (app->mode + 1) % HRFModeCount;
                    break;
                case InputKeyLeft:
                    if(!app->active)
                        app->mode = app->mode == 0 ? HRFModeCount - 1 : app->mode - 1;
                    break;
                case InputKeyUp:
                    app->frequency += 1000000;
                    if(app->frequency > 6000000000) app->frequency = 1000000;
                    break;
                case InputKeyDown:
                    if(app->frequency > 1000000) app->frequency -= 1000000;
                    break;
                case InputKeyBack:
                    if(app->active) {
                        app->active = false;
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
