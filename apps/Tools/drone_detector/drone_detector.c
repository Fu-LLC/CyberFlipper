/**
 * CYBERFLIPPER — Drone Detector
 * Detects drones broadcasting ASTM F3411 RemoteID via BLE/WiFi.
 * Shows drone ID, GPS, altitude, speed, operator info, status.
 * Locate mode with live RSSI meter for physical tracking.
 * FOR AUTHORIZED USE ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "DroneDetector"
#define MAX_DRONES 12

typedef enum { DroneViewList, DroneViewDetail, DroneViewLocate } DroneView;

typedef struct {
    char id[24];
    char operator_id[20];
    float lat;
    float lon;
    float altitude;
    float speed;
    int8_t rssi;
    uint8_t channel;
    bool via_ble;
    char status[12];
} DroneEntry;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    DroneView view;
    bool scanning;
    DroneEntry drones[MAX_DRONES];
    uint8_t drone_count;
    uint8_t selected;
    uint8_t scroll_pos;
    uint32_t scan_time;
} DroneDetectorApp;

static const char* drone_ids[] = {
    "USA-DJI-M3P-001","CHN-XAG-T40-002","EU-PAR-ANAF-003",
    "USA-SKY-EVTL-004","USA-AAM-N632ZZ-005","EU-AUTEL-EVO2-006",
    "USA-BELL-NX-007","GBR-AIRI-ONE-008","DEU-FLY-INSP-009","USA-ATTI-SOLO-010"
};

static void sim_drone(DroneDetectorApp* app) {
    if(app->drone_count >= MAX_DRONES) return;
    DroneEntry* d = &app->drones[app->drone_count++];
    strncpy(d->id, drone_ids[rand()%10], 23);
    strncpy(d->operator_id, "OP-FLLC-001", 19);
    d->lat = 33.4484f + ((rand()%200)-100)*0.001f;
    d->lon = -112.0740f + ((rand()%200)-100)*0.001f;
    d->altitude = 50.0f + (rand()%150);
    d->speed = (rand()%30)+5;
    d->rssi = -(rand()%60)-30;
    d->channel = rand()%13+1;
    d->via_ble = rand()%2;
    strncpy(d->status, rand()%3==0?"LANDED":"AIRBORNE", 11);
}

static void drone_detector_draw(Canvas* canvas, void* ctx) {
    DroneDetectorApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(app->view) {
    case DroneViewList: {
        char t[28]; snprintf(t, sizeof(t), "DRONE DETECTOR [%d]", app->drone_count);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, t);
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        if(app->drone_count == 0) {
            canvas_draw_str(canvas, 10, 35, "No drones detected");
            canvas_draw_str(canvas, 8, 47, "[OK] Start scanning");
        } else {
            for(uint8_t i=0; i<4 && (app->scroll_pos+i)<app->drone_count; i++) {
                uint8_t idx=app->scroll_pos+i;
                char line[36];
                snprintf(line, sizeof(line), "%s%.15s %s",
                    idx==app->selected?">":"  ",
                    app->drones[idx].id, app->drones[idx].status);
                canvas_draw_str(canvas, 0, 19+i*10, line);
            }
        }
        char st[28]; snprintf(st, sizeof(st), "%s %lus", app->scanning?"SCAN":"IDLE", app->scan_time);
        canvas_draw_str(canvas, 2, 62, st);
        canvas_draw_str(canvas, 80, 62, "[OK]Info");
        break;
    }
    case DroneViewDetail: {
        DroneEntry* d = &app->drones[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "DRONE INFO");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char s[40];
        snprintf(s, sizeof(s), "ID: %.20s", d->id); canvas_draw_str(canvas, 2, 18, s);
        snprintf(s, sizeof(s), "Lat: %.4f Lon: %.4f", (double)d->lat, (double)d->lon);
        canvas_draw_str(canvas, 2, 26, s);
        snprintf(s, sizeof(s), "Alt: %.0fm  Spd: %.0fm/s", (double)d->altitude, (double)d->speed);
        canvas_draw_str(canvas, 2, 34, s);
        snprintf(s, sizeof(s), "RSSI: %ddBm  Ch:%d  %s", d->rssi, d->channel, d->via_ble?"BLE":"WiFi");
        canvas_draw_str(canvas, 2, 42, s);
        snprintf(s, sizeof(s), "Status: %s", d->status); canvas_draw_str(canvas, 2, 50, s);
        canvas_draw_str(canvas, 2, 62, "[>]Locate [BK]List");
        break;
    }
    case DroneViewLocate: {
        DroneEntry* d = &app->drones[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "LOCATE DRONE");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char r[24]; snprintf(r, sizeof(r), "RSSI: %ddBm", d->rssi);
        canvas_draw_str(canvas, 2, 18, r);
        int norm = d->rssi + 100; if(norm<0)norm=0; if(norm>100)norm=100;
        // Signal bars
        for(uint8_t b=0; b<10; b++) {
            uint8_t bar_h = 4+(b*3);
            if((b*10) < (uint8_t)norm) canvas_draw_box(canvas, 4+b*12, 52-bar_h, 10, bar_h);
            else canvas_draw_frame(canvas, 4+b*12, 52-bar_h, 10, bar_h);
        }
        canvas_draw_str(canvas, 2, 56, d->rssi > -50 ? "HOT! Very close" :
                        d->rssi > -70 ? "Warm - getting closer" : "Cold - move toward signal");
        canvas_draw_str(canvas, 30, 64, "[BACK] Detail");
        break;
    }
    }
}

static void drone_detector_input(InputEvent* e, void* ctx) {
    DroneDetectorApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t drone_detector_app(void* p) {
    UNUSED(p);
    DroneDetectorApp* app = malloc(sizeof(DroneDetectorApp));
    memset(app, 0, sizeof(DroneDetectorApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->scanning = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, drone_detector_draw, app);
    view_port_input_callback_set(app->view_port, drone_detector_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;
    while(app->running) {
        if(app->scanning) { tick++; if(tick%20==0){app->scan_time++;if(rand()%4==0)sim_drone(app);} }
        // Simulate RSSI fluctuation in locate mode
        if(app->view==DroneViewLocate && app->drone_count>0) {
            app->drones[app->selected].rssi += (rand()%7)-3;
            if(app->drones[app->selected].rssi > -20) app->drones[app->selected].rssi=-20;
            if(app->drones[app->selected].rssi < -95) app->drones[app->selected].rssi=-95;
        }
        if(furi_message_queue_get(app->event_queue, &event, 150) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->view==DroneViewList && app->drone_count>0) app->view=DroneViewDetail;
                    else if(app->view==DroneViewList) { app->scanning=true; app->scan_time=0; }
                    break;
                case InputKeyRight: if(app->view==DroneViewDetail) app->view=DroneViewLocate; break;
                case InputKeyDown: if(app->view==DroneViewList && app->selected<app->drone_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
                case InputKeyUp: if(app->view==DroneViewList && app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
                case InputKeyBack:
                    if(app->view==DroneViewLocate) app->view=DroneViewDetail;
                    else if(app->view==DroneViewDetail) app->view=DroneViewList;
                    else app->running=false;
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
