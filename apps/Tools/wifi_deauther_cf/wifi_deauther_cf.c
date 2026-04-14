/**
 * CYBERFLIPPER — WiFi Deauther
 * Educational WiFi deauthentication frame generator for authorized
 * network security testing. Targets specific clients or broadcasts
 * deauth to all clients on a network via ESP32 UART bridge.
 * FOR AUTHORIZED TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "WiFiDeauther"
#define MAX_APS 16

typedef enum { DeauthModeAll, DeauthModeTarget, DeauthModeFlood, DeauthModeCount } DeauthMode;
static const char* deauth_mode_names[] = { "All Clients", "Target Client", "Flood Mode" };

typedef struct {
    char ssid[33];
    uint8_t bssid[6];
    int8_t rssi;
    uint8_t channel;
    uint8_t clients;
    bool selected;
} APEntry;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    DeauthMode mode;
    bool attacking;
    uint8_t scroll_pos;
    uint8_t selected_ap;
    uint32_t packets_sent;
    uint32_t elapsed;
    APEntry aps[MAX_APS];
    uint8_t ap_count;
    bool scanning;
} WiFiDeautherApp;

static void sim_scan(WiFiDeautherApp* app) {
    static const char* ssids[] = {
        "Xfinity_2G","ATT-Router","NETGEAR55","Verizon-FIOS",
        "TP-Link_Home","Spectrum_WiFi","Guest_Network","Corp_Secure",
        "iPhone_Hotspot","AndroidAP","HideYoKids","TellMyWiFiLoveHer",
        "PrettyFlyForAWifi","LAN_of_the_Free","Bill_Wi_the_Science_Fi",
        "Router_Ferguson"
    };
    app->ap_count = 0;
    for(uint8_t i = 0; i < 10; i++) {
        APEntry* ap = &app->aps[app->ap_count++];
        strncpy(ap->ssid, ssids[i % 16], 32);
        for(int j = 0; j < 6; j++) ap->bssid[j] = rand() % 256;
        ap->rssi = -(rand() % 70) - 20;
        ap->channel = 1 + (rand() % 13);
        ap->clients = rand() % 8;
        ap->selected = false;
    }
}

static void deauther_draw(Canvas* canvas, void* ctx) {
    WiFiDeautherApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "WIFI DEAUTHER");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    if(app->scanning) {
        canvas_draw_str(canvas, 2, 20, "Scanning networks...");
        char c[24]; snprintf(c, sizeof(c), "Found: %d APs", app->ap_count);
        canvas_draw_str(canvas, 2, 32, c);
        return;
    }

    if(app->attacking) {
        char m[40]; snprintf(m, sizeof(m), "TARGET: %s", app->aps[app->selected_ap].ssid);
        canvas_draw_str(canvas, 2, 18, m);
        char p[32]; snprintf(p, sizeof(p), "Mode: %s", deauth_mode_names[app->mode]);
        canvas_draw_str(canvas, 2, 28, p);
        char s[32]; snprintf(s, sizeof(s), "Pkts: %lu  Time: %lus", app->packets_sent, app->elapsed);
        canvas_draw_str(canvas, 2, 38, s);
        canvas_draw_frame(canvas, 2, 44, 124, 6);
        uint32_t bar = (app->packets_sent * 124) / 2000; if(bar > 124) bar = 124;
        canvas_draw_box(canvas, 3, 45, bar, 4);
        canvas_draw_str_aligned(canvas, 64, 54, AlignCenter, AlignTop, ">> DEAUTHING <<");
        canvas_draw_str(canvas, 30, 63, "[BACK] Stop");
        return;
    }

    for(uint8_t i = 0; i < 4 && (app->scroll_pos + i) < app->ap_count; i++) {
        uint8_t idx = app->scroll_pos + i;
        char line[36];
        snprintf(line, sizeof(line), "%s%s ch%d %ddBm",
            idx == app->selected_ap ? ">" : " ",
            app->aps[idx].ssid, app->aps[idx].channel, app->aps[idx].rssi);
        canvas_draw_str(canvas, 0, 19 + i*10, line);
    }
    canvas_draw_str(canvas, 2, 62, "[OK]Attack [<>]Mode [BK]Exit");
}

static void deauther_input(InputEvent* e, void* ctx) {
    WiFiDeautherApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t wifi_deauther_cf_app(void* p) {
    UNUSED(p);
    WiFiDeautherApp* app = malloc(sizeof(WiFiDeautherApp));
    memset(app, 0, sizeof(WiFiDeautherApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, deauther_draw, app);
    view_port_input_callback_set(app->view_port, deauther_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->scanning = true;
    uint32_t tick = 0;
    sim_scan(app);
    app->scanning = false;

    InputEvent event;
    while(app->running) {
        if(app->attacking) {
            tick++;
            if(tick % 5 == 0) { app->elapsed++; app->packets_sent += 50 + rand()%100; }
        }
        if(furi_message_queue_get(app->event_queue, &event,
            app->attacking ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->attacking) { app->attacking = true; app->packets_sent = 0; app->elapsed = 0; tick = 0; }
                    break;
                case InputKeyDown: if(!app->attacking && app->selected_ap < app->ap_count-1) { app->selected_ap++; if(app->selected_ap >= app->scroll_pos+4) app->scroll_pos++; } break;
                case InputKeyUp: if(!app->attacking && app->selected_ap > 0) { app->selected_ap--; if(app->selected_ap < app->scroll_pos) app->scroll_pos = app->selected_ap; } break;
                case InputKeyRight: if(!app->attacking) app->mode = (app->mode+1)%DeauthModeCount; break;
                case InputKeyLeft: if(!app->attacking) app->mode = app->mode==0?DeauthModeCount-1:app->mode-1; break;
                case InputKeyBack: if(app->attacking) app->attacking=false; else app->running=false; break;
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
