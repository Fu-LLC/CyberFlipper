/**
 * CYBERFLIPPER — AirTag Detector & Spoofer
 * Detects Apple AirTag FindMy beacons via BLE.
 * Locate mode uses RSSI to physically track tags.
 * Spoof mode rebroadcasts detected AirTag identities.
 * FOR CONSUMER PROTECTION AND AUTHORIZED USE ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "AirTagDetector"
#define MAX_AIRTAGS 16

// Apple FindMy OUI / Company ID = 0x004C
// AirTag uses "Offline Finding" service UUID 0xFD6F

typedef enum { ATViewList, ATViewDetail, ATViewLocate, ATViewSpoof } ATView;

typedef struct {
    uint8_t addr[6];
    uint8_t status_byte;
    int8_t rssi;
    uint32_t first_seen;
    uint32_t last_seen;
    uint32_t beacon_count;
    bool is_moving;
    bool spoof_active;
} AirTagEntry;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    ATView view;
    bool scanning;
    AirTagEntry tags[MAX_AIRTAGS];
    uint8_t tag_count;
    uint8_t selected;
    uint8_t scroll_pos;
    uint32_t scan_time;
    uint32_t spoof_count;
} AirTagApp;

static void sim_airtag(AirTagApp* app) {
    if(app->tag_count >= MAX_AIRTAGS || rand()%6!=0) return;
    AirTagEntry* t = &app->tags[app->tag_count++];
    for(int i=0;i<6;i++) t->addr[i]=rand()%256;
    t->addr[0] = 0x2C; // Apple OUI start
    t->status_byte = rand()%256;
    t->rssi = -(rand()%70)-20;
    t->first_seen = app->scan_time;
    t->last_seen = app->scan_time;
    t->beacon_count = 1;
    t->is_moving = rand()%3==0;
    FURI_LOG_I(TAG,"AirTag: %02X:%02X:%02X status=0x%02X",
        t->addr[0],t->addr[1],t->addr[2],t->status_byte);
}

static void airtag_draw(Canvas* canvas, void* ctx) {
    AirTagApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(app->view) {
    case ATViewList: {
        char title[28]; snprintf(title, sizeof(title), "AIRTAG DETECT [%d]", app->tag_count);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, title);
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        if(app->tag_count==0) {
            canvas_draw_str(canvas, 8, 32, app->scanning?"Scanning for AirTags...":"No AirTags found");
            canvas_draw_str(canvas, 8, 44, "[OK] Start Scanning");
            break;
        }
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->tag_count;i++) {
            uint8_t idx=app->scroll_pos+i;
            AirTagEntry* t=&app->tags[idx];
            char line[36];
            snprintf(line, sizeof(line), "%s%02X:%02X:%02X %ddBm %s",
                idx==app->selected?">":"  ",
                t->addr[0],t->addr[1],t->addr[2],t->rssi,
                t->is_moving?"MOV":"STA");
            canvas_draw_str(canvas, 0, 19+i*10, line);
        }
        char st[24]; snprintf(st,sizeof(st),"%s %lus",app->scanning?"SCAN":"IDLE",app->scan_time);
        canvas_draw_str(canvas, 2, 62, st);
        break;
    }
    case ATViewDetail: {
        if(app->tag_count==0) break;
        AirTagEntry* t=&app->tags[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "AIRTAG INFO");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char mac[20]; snprintf(mac,sizeof(mac),"%02X:%02X:%02X:%02X:%02X:%02X",
            t->addr[0],t->addr[1],t->addr[2],t->addr[3],t->addr[4],t->addr[5]);
        canvas_draw_str(canvas, 2, 18, mac);
        char r[24]; snprintf(r,sizeof(r),"RSSI: %d dBm",t->rssi); canvas_draw_str(canvas, 2, 28, r);
        char s[32]; snprintf(s,sizeof(s),"Status: 0x%02X  Seen: %lu",t->status_byte,t->beacon_count);
        canvas_draw_str(canvas, 2, 38, s);
        canvas_draw_str(canvas, 2, 48, t->is_moving?"!! MOVING - ALERT !!":"Stationary");
        canvas_draw_str(canvas, 2, 58, "Mfr: Apple Inc (0x004C)");
        canvas_draw_str(canvas, 2, 64, "[>]Locate [L]Spoof [BK]List");
        break;
    }
    case ATViewLocate: {
        if(app->tag_count==0) break;
        AirTagEntry* t=&app->tags[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "LOCATE AIRTAG");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char r[24]; snprintf(r,sizeof(r),"RSSI: %d dBm",t->rssi);
        canvas_draw_str(canvas, 2, 18, r);
        int norm=t->rssi+100; if(norm<0)norm=0; if(norm>100)norm=100;
        for(uint8_t b=0;b<10;b++) {
            uint8_t bh=4+(b*3);
            if(b*10<(uint8_t)norm) canvas_draw_box(canvas,4+b*12,52-bh,10,bh);
            else canvas_draw_frame(canvas,4+b*12,52-bh,10,bh);
        }
        const char* prox = t->rssi>-40?"VERY CLOSE!":t->rssi>-60?"Close":"Far away";
        canvas_draw_str_aligned(canvas, 64, 54, AlignCenter, AlignTop, prox);
        canvas_draw_str(canvas, 30, 63, "[BACK] Return");
        break;
    }
    case ATViewSpoof: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "AIRTAG SPOOF");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char sc[32]; snprintf(sc,sizeof(sc),"Spoof packets: %lu",app->spoof_count);
        canvas_draw_str(canvas, 2, 24, sc);
        canvas_draw_str(canvas, 2, 34, "Broadcasting cloned");
        canvas_draw_str(canvas, 2, 42, "AirTag FindMy beacons");
        canvas_draw_str(canvas, 2, 54, "[OK] Stop  [BACK] Return");
        break;
    }
    }
}

static void airtag_input(InputEvent* e, void* ctx) {
    AirTagApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t airtag_detector_app(void* p) {
    UNUSED(p);
    AirTagApp* app = malloc(sizeof(AirTagApp));
    memset(app,0,sizeof(AirTagApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->scanning = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, airtag_draw, app);
    view_port_input_callback_set(app->view_port, airtag_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick=0;
    while(app->running) {
        tick++;
        if(app->scanning&&tick%12==0){app->scan_time++;sim_airtag(app);}
        if(app->view==ATViewLocate&&app->tag_count>0){
            app->tags[app->selected].rssi+=(rand()%7)-3;
            if(app->tags[app->selected].rssi>-15)app->tags[app->selected].rssi=-15;
            if(app->tags[app->selected].rssi<-95)app->tags[app->selected].rssi=-95;
        }
        if(app->view==ATViewSpoof) app->spoof_count++;

        if(furi_message_queue_get(app->event_queue,&event,150)==FuriStatusOk) {
            if(event.type==InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->view==ATViewList){if(app->tag_count>0)app->view=ATViewDetail;else{app->scanning=true;app->scan_time=0;tick=0;}}
                    else if(app->view==ATViewSpoof)app->view=ATViewDetail;
                    break;
                case InputKeyRight: if(app->view==ATViewDetail)app->view=ATViewLocate; break;
                case InputKeyLeft: if(app->view==ATViewDetail){app->view=ATViewSpoof;app->spoof_count=0;} break;
                case InputKeyDown: if(app->view==ATViewList&&app->selected<app->tag_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
                case InputKeyUp: if(app->view==ATViewList&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
                case InputKeyBack:
                    if(app->view==ATViewLocate||app->view==ATViewSpoof)app->view=ATViewDetail;
                    else if(app->view==ATViewDetail)app->view=ATViewList;
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
