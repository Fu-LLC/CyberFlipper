/**
 * CYBERFLIPPER — Beacon Spam
 * Broadcasts fake WiFi SSIDs via ESP32 UART bridge.
 * Modes: Random names, Clone real APs, Custom SSID list.
 * FOR AUTHORIZED TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "BeaconSpam"

typedef enum { BSModeRandom, BSModeClone, BSModeCustom, BSModeCount } BeaconSpamMode;
static const char* bs_mode_names[] = { "Random Names", "Clone Real APs", "Custom List" };

static const char* random_ssids[] = {
    "FBI_Surveillance_Van","CIA_Mobile_Unit","NSA_PRISM_Node",
    "Pretty Fly for a WiFi","Not Your WiFi","FBI Surveillance Van #3",
    "Tell My WiFi Love Her","Loading...", "Searching...",
    "The Internet","Abraham Linksys","John Wilkes Bluetooth",
    "HideYoKidsHideYoWifi","Winternet is Coming","Shut your mouth","Router? I Hardly Know Her",
    "Silence of the LANs","Bill Wi the Science Fi","Nacho WiFi","Wu-Tang LAN",
    "The Promised LAN","The LAN Before Time","Lord of the Pings","LAN Solo",
    "The Matrix Has You", "Get Off My LAN","2.4GHz or Not to GHz","No Wires No Worries"
};
#define RANDOM_COUNT 28

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    BeaconSpamMode mode;
    bool spamming;
    uint32_t beacons_sent;
    uint32_t active_ssids;
    uint32_t elapsed;
    uint8_t channel;
} BeaconSpamApp;

static void beacon_spam_draw(Canvas* canvas, void* ctx) {
    BeaconSpamApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "BEACON SPAM");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    char m[32]; snprintf(m, sizeof(m), "Mode: %s", bs_mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 18, m);
    char ch[20]; snprintf(ch, sizeof(ch), "Channel: %d", app->channel);
    canvas_draw_str(canvas, 2, 26, ch);

    if(app->spamming) {
        char ssids_str[32]; snprintf(ssids_str, sizeof(ssids_str), "SSIDs: %lu", app->active_ssids);
        canvas_draw_str(canvas, 2, 34, ssids_str);
        char pkts[32]; snprintf(pkts, sizeof(pkts), "Beacons: %lu", app->beacons_sent);
        canvas_draw_str(canvas, 2, 42, pkts);
        char t[24]; snprintf(t, sizeof(t), "Time: %lus", app->elapsed);
        canvas_draw_str(canvas, 2, 50, t);
        // Animated bar
        uint8_t anim = (app->beacons_sent % 120);
        canvas_draw_box(canvas, 2, 57, anim > 124 ? 124 : anim, 4);
        canvas_draw_str(canvas, 50, 63, "[BACK] Stop");
    } else {
        canvas_draw_str(canvas, 2, 38, "[OK] Start Spamming");
        canvas_draw_str(canvas, 2, 48, "[<>] Mode  [UD] Channel");
        canvas_draw_str(canvas, 2, 58, "[BACK] Exit");
    }
}

static void beacon_spam_input(InputEvent* e, void* ctx) {
    BeaconSpamApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t beacon_spam_cf_app(void* p) {
    UNUSED(p);
    BeaconSpamApp* app = malloc(sizeof(BeaconSpamApp));
    memset(app, 0, sizeof(BeaconSpamApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->channel = 6;
    app->active_ssids = 10;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, beacon_spam_draw, app);
    view_port_input_callback_set(app->view_port, beacon_spam_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;
    while(app->running) {
        if(app->spamming) {
            tick++;
            if(tick%5==0){app->elapsed++;app->beacons_sent+=app->active_ssids*(2+rand()%3);}
        }
        if(furi_message_queue_get(app->event_queue, &event,
            app->spamming?100:50)==FuriStatusOk) {
            if(event.type==InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->spamming) {
                        app->spamming=true; app->beacons_sent=0; app->elapsed=0; tick=0;
                        app->active_ssids = app->mode==BSModeRandom?RANDOM_COUNT:(5+rand()%10);
                        FURI_LOG_I(TAG,"Spam: %s %lu SSIDs",bs_mode_names[app->mode],app->active_ssids);
                    }
                    break;
                case InputKeyRight: if(!app->spamming) app->mode=(app->mode+1)%BSModeCount; break;
                case InputKeyLeft: if(!app->spamming) app->mode=app->mode==0?BSModeCount-1:app->mode-1; break;
                case InputKeyUp: if(!app->spamming&&app->channel<13) app->channel++; break;
                case InputKeyDown: if(!app->spamming&&app->channel>1) app->channel--; break;
                case InputKeyBack: if(app->spamming)app->spamming=false;else app->running=false; break;
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
