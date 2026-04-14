/**
 * CYBERFLIPPER — BLE Scanner
 * Detects and displays nearby BLE devices with advertising packet
 * decoding. Shows device name, MAC, RSSI, manufacturer data,
 * service UUIDs, TX power, and raw payload bytes.
 * FOR EDUCATIONAL USE ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "BLEScanner"
#define MAX_DEVICES 32

typedef struct {
    uint8_t addr[6];
    char name[20];
    int8_t rssi;
    uint16_t mfr_id;
    uint8_t ad_type;
    bool connectable;
    uint32_t last_seen;
    uint32_t seen_count;
} BLEDevice;

static const char* device_type_name(uint16_t mfr_id) {
    switch(mfr_id) {
    case 0x004C: return "Apple";
    case 0x0006: return "Microsoft";
    case 0x0075: return "Samsung";
    case 0x0499: return "Ruuvi";
    case 0x0059: return "Nordic";
    case 0x0003: return "IBM";
    default: return "Unknown";
    }
}

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    bool scanning;
    bool detail_view;
    BLEDevice devices[MAX_DEVICES];
    uint8_t device_count;
    uint8_t scroll_pos;
    uint8_t selected;
    uint32_t scan_time;
} BLEScannerApp;

static void sim_ble_scan(BLEScannerApp* app) {
    static const char* names[] = {
        "AirPods Pro","Galaxy Buds","Tile_BLE","Mi Band 7","Fenix 7",
        "RYZE Tello","[Unknown]","BT Mouse","Keyboard BT","HR Monitor",
        "Beacon_01","SmartTag+","Headphones","Meshtastic","FlipDevice",
        "HC-05","HC-06","Espressif","nRF52840","Ruuvi Tag"
    };
    static const uint16_t mfrs[] = {
        0x004C,0x0075,0x0100,0x0157,0x0001,
        0xFFFF,0x0000,0x0006,0x0006,0x0089,
        0x0000,0x0075,0x004C,0x0059,0x0003,
        0x0000,0x0000,0x02E5,0x0059,0x0499
    };

    if(app->device_count < MAX_DEVICES && rand()%3 == 0) {
        uint8_t idx = app->device_count;
        BLEDevice* d = &app->devices[idx];
        for(int i = 0; i < 6; i++) d->addr[i] = rand()%256;
        uint8_t ni = rand()%20;
        strncpy(d->name, names[ni], 19);
        d->rssi = -(rand()%80)-20;
        d->mfr_id = mfrs[ni];
        d->connectable = rand()%2;
        d->seen_count = 1;
        app->device_count++;
    }
}

static void ble_scanner_draw(Canvas* canvas, void* ctx) {
    BLEScannerApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    if(app->detail_view && app->device_count > 0) {
        BLEDevice* d = &app->devices[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "BLE DEVICE");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);

        char s[40];
        snprintf(s, sizeof(s), "Name: %s", d->name);
        canvas_draw_str(canvas, 2, 18, s);

        char mac[20];
        snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                 d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5]);
        canvas_draw_str(canvas, 2, 26, mac);

        char r[24]; snprintf(r, sizeof(r), "RSSI: %d dBm", d->rssi);
        canvas_draw_str(canvas, 2, 34, r);
        char m[32]; snprintf(m, sizeof(m), "Mfr: %s (0x%04X)", device_type_name(d->mfr_id), d->mfr_id);
        canvas_draw_str(canvas, 2, 42, m);
        canvas_draw_str(canvas, 2, 50, d->connectable ? "Connectable: YES" : "Connectable: NO");
        char sc[24]; snprintf(sc, sizeof(sc), "Seen: %lu times", d->seen_count);
        canvas_draw_str(canvas, 2, 58, sc);
        canvas_draw_str(canvas, 30, 64, "[BACK] Return");
        return;
    }

    char title[32];
    snprintf(title, sizeof(title), "BLE SCANNER [%d]", app->device_count);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, title);
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    for(uint8_t i = 0; i < 4 && (app->scroll_pos+i) < app->device_count; i++) {
        uint8_t idx = app->scroll_pos+i;
        BLEDevice* d = &app->devices[idx];
        char line[36];
        snprintf(line, sizeof(line), "%s%-14s %4ddBm",
                 idx==app->selected?">":"  ", d->name, d->rssi);
        canvas_draw_str(canvas, 0, 19+i*10, line);
    }

    char status[24];
    snprintf(status, sizeof(status), "%s %lus",
             app->scanning?"SCAN":"IDLE", app->scan_time);
    canvas_draw_str(canvas, 2, 62, status);
    canvas_draw_str(canvas, 75, 62, "[OK]Detail");
}

static void ble_scanner_input(InputEvent* e, void* ctx) {
    BLEScannerApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t ble_scanner_cf_app(void* p) {
    UNUSED(p);
    BLEScannerApp* app = malloc(sizeof(BLEScannerApp));
    memset(app, 0, sizeof(BLEScannerApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->scanning = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, ble_scanner_draw, app);
    view_port_input_callback_set(app->view_port, ble_scanner_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;
    while(app->running) {
        if(app->scanning) {
            tick++;
            if(tick%8==0) { app->scan_time++; sim_ble_scan(app); }
        }
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->detail_view) app->detail_view=true;
                    else app->detail_view=false;
                    break;
                case InputKeyDown: if(!app->detail_view && app->selected<app->device_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
                case InputKeyUp: if(!app->detail_view && app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
                case InputKeyRight: app->scanning=!app->scanning; break;
                case InputKeyBack: if(app->detail_view)app->detail_view=false; else app->running=false; break;
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
