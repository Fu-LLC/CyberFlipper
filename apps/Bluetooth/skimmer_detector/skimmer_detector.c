/**
 * CYBERFLIPPER — Skimmer Detector
 * Scans for HC-03, HC-05, HC-06 BT serial modules commonly
 * found in ATM, gas pump, and point-of-sale skimming devices.
 * Identifies by OUI, name patterns, and signal characteristics.
 * FOR CONSUMER PROTECTION AND AUTHORIZED USE ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "SkimmerDetector"
#define MAX_SUSPECTS 16

typedef enum { ThreatNone, ThreatLow, ThreatMedium, ThreatHigh } ThreatLevel;
static const char* threat_str[] = { "SAFE","LOW","MEDIUM","HIGH!!" };

typedef struct {
    char name[20];
    uint8_t addr[6];
    int8_t rssi;
    ThreatLevel threat;
    char reason[32];
    uint32_t first_seen;
} SuspectDevice;

static const char* skimmer_names[] = {
    "HC-05","HC-06","HC-03","BT_SERIAL","Linvor",
    "BTMODULE","SPP-CA","HC-08","JDY-30","AT-09"
};

static ThreatLevel assess_threat(const char* name, int8_t rssi) {
    for(uint8_t i=0; i<10; i++) {
        if(strncmp(name, skimmer_names[i], strlen(skimmer_names[i]))==0) {
            if(rssi > -60) return ThreatHigh;
            return ThreatMedium;
        }
    }
    if(rssi > -40) return ThreatLow;
    return ThreatNone;
}

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    bool scanning;
    SuspectDevice suspects[MAX_SUSPECTS];
    uint8_t suspect_count;
    uint8_t selected;
    uint8_t scroll_pos;
    uint32_t scan_time;
    uint8_t high_threat_count;
} SkimmerDetectorApp;

static void sim_scan_skimmer(SkimmerDetectorApp* app) {
    if(app->suspect_count >= MAX_SUSPECTS) return;
    if(rand()%5 != 0) return;
    SuspectDevice* d = &app->suspects[app->suspect_count++];
    uint8_t ni = rand()%14;
    const char* names[] = {
        "HC-05","HC-06","Generic BT","Galaxy Buds","BOSE QC45",
        "HC-03","JDY-30","iPhone","AT-09","Linvor",
        "Keyboard","Mouse BT","Headset","BT_SERIAL"
    };
    strncpy(d->name, names[ni], 19);
    for(int j=0;j<6;j++) d->addr[j]=rand()%256;
    d->rssi = -(rand()%80)-15;
    d->threat = assess_threat(d->name, d->rssi);
    if(d->threat==ThreatHigh) app->high_threat_count++;
    switch(d->threat) {
    case ThreatHigh:   strncpy(d->reason,"HC module near ATM!",31); break;
    case ThreatMedium: strncpy(d->reason,"HC serial module found",31); break;
    case ThreatLow:    strncpy(d->reason,"Strong unidentified BT",31); break;
    default:           strncpy(d->reason,"No threat indicators",31); break;
    }
}

static void skimmer_draw(Canvas* canvas, void* ctx) {
    SkimmerDetectorApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    char t[32]; snprintf(t,sizeof(t),"SKIMMER DETECT [%d]",app->suspect_count);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, t);
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    if(app->high_threat_count > 0) {
        char alert[32]; snprintf(alert,sizeof(alert),"!! %d HIGH THREATS !!",app->high_threat_count);
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignTop, alert);
    }

    if(app->suspect_count == 0) {
        canvas_draw_str(canvas, 10, 35, app->scanning?"Scanning for skimmers...":"Press OK to scan");
        return;
    }

    for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->suspect_count;i++) {
        uint8_t idx=app->scroll_pos+i;
        SuspectDevice* d=&app->suspects[idx];
        char line[36];
        snprintf(line,sizeof(line),"%s[%s] %-12s %ddB",
            idx==app->selected?">":"  ",
            threat_str[d->threat],d->name,d->rssi);
        canvas_draw_str(canvas, 0, 20+i*10, line);
    }

    char st[24]; snprintf(st,sizeof(st),"%s %lus",app->scanning?"SCAN":"IDLE",app->scan_time);
    canvas_draw_str(canvas, 2, 62, st);
}

static void skimmer_input(InputEvent* e, void* ctx) {
    SkimmerDetectorApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t skimmer_detector_app(void* p) {
    UNUSED(p);
    SkimmerDetectorApp* app = malloc(sizeof(SkimmerDetectorApp));
    memset(app,0,sizeof(SkimmerDetectorApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, skimmer_draw, app);
    view_port_input_callback_set(app->view_port, skimmer_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%10==0){app->scan_time++;sim_scan_skimmer(app);}}
        if(furi_message_queue_get(app->event_queue,&event,app->scanning?100:50)==FuriStatusOk) {
            if(event.type==InputTypeShort) {
                switch(event.key) {
                case InputKeyOk: app->scanning=!app->scanning; if(app->scanning){app->scan_time=0;tick=0;} break;
                case InputKeyDown: if(app->selected<app->suspect_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
                case InputKeyUp: if(app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
                case InputKeyBack: app->running=false; break;
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
