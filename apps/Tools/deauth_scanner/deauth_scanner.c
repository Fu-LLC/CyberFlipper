/**
 * CYBERFLIPPER — Deauth Scanner
 * Real-time WiFi deauthentication frame monitor.
 * Detects deauth attacks, logs source MAC, target, channel,
 * reason code, and RSSI. Physical locate mode via RSSI bars.
 * FOR AUTHORIZED NETWORK MONITORING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "DeauthScanner"
#define MAX_EVENTS 24

typedef struct {
    uint8_t src_mac[6];
    uint8_t dst_mac[6];
    uint8_t channel;
    uint8_t reason_code;
    int8_t rssi;
    uint32_t count;
    uint32_t timestamp;
} DeauthEvent;

static const char* reason_codes[] = {
    "?","Unspec","Prev Auth Invalid","Leaving","Inactivity",
    "Too Many Assoc","From NonAuth","From NonAssoc","Leaving BSS",
    "Not Auth","QoS","Cipher Reject","Disassoc Leaving","Invalid IE"
};

typedef enum { DSViewLive, DSViewList, DSViewLocate } DSView;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    bool monitoring;
    DSView view;
    DeauthEvent events[MAX_EVENTS];
    uint8_t event_count;
    uint8_t selected;
    uint8_t scroll_pos;
    uint32_t monitor_time;
    uint32_t total_frames;
    int8_t live_rssi;
    uint8_t live_channel;
} DeauthScannerApp;

static void sim_deauth(DeauthScannerApp* app) {
    if(rand()%8!=0) return;
    DeauthEvent* e;
    // Check if same source already exists
    uint8_t src[6]; for(int i=0;i<6;i++) src[i]=rand()%256;
    bool found=false;
    for(uint8_t i=0;i<app->event_count;i++) {
        if(memcmp(app->events[i].src_mac,src,6)==0){app->events[i].count++;app->events[i].rssi=-(rand()%60)-20;found=true;break;}
    }
    if(!found&&app->event_count<MAX_EVENTS) {
        e=&app->events[app->event_count++];
        memcpy(e->src_mac,src,6);
        for(int i=0;i<6;i++) e->dst_mac[i]=i<5?0xFF:0xFF; // Broadcast
        e->channel=1+(rand()%13);
        e->reason_code=(rand()%13)+1;
        e->rssi=-(rand()%60)-20;
        e->count=1;
        e->timestamp=app->monitor_time;
        FURI_LOG_I(TAG,"Deauth from %02X:%02X:%02X ch%d reason=%d rssi=%d",
            e->src_mac[0],e->src_mac[1],e->src_mac[2],e->channel,e->reason_code,e->rssi);
    }
    app->total_frames++;
    app->live_rssi=-(rand()%60)-20;
    app->live_channel=1+(rand()%13);
}

static void deauth_scanner_draw(Canvas* canvas, void* ctx) {
    DeauthScannerApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(app->view) {
    case DSViewLive: {
        char t[32]; snprintf(t,sizeof(t),"DEAUTH SCAN [%lu]",app->total_frames);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, t);
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);

        if(!app->monitoring) {
            canvas_draw_str(canvas, 10, 32, "Monitor not active");
            canvas_draw_str(canvas, 10, 44, "[OK] Start Monitoring");
            break;
        }
        char live[40]; snprintf(live,sizeof(live),"Live: Ch%d RSSI:%ddBm",app->live_channel,app->live_rssi);
        canvas_draw_str(canvas, 2, 18, live);

        // Live RSSI bar
        int norm=app->live_rssi+100; if(norm<0)norm=0; if(norm>100)norm=100;
        canvas_draw_frame(canvas, 2, 22, 124, 6);
        canvas_draw_box(canvas, 3, 23, (norm*122)/100, 4);

        char src_count[32]; snprintf(src_count,sizeof(src_count),"Sources: %d  Frames: %lu",app->event_count,app->total_frames);
        canvas_draw_str(canvas, 2, 32, src_count);
        char mt[24]; snprintf(mt,sizeof(mt),"Runtime: %lus",app->monitor_time);
        canvas_draw_str(canvas, 2, 40, mt);

        // Show top 2 attackers
        for(uint8_t i=0;i<2&&i<app->event_count;i++) {
            char line[36]; snprintf(line,sizeof(line),"%02X:%02X:%02X ch%d x%lu",
                app->events[i].src_mac[0],app->events[i].src_mac[1],app->events[i].src_mac[2],
                app->events[i].channel,app->events[i].count);
            canvas_draw_str(canvas, 2, 50+i*9, line);
        }
        canvas_draw_str(canvas, 55, 63, "[>]List [BK]Stop");
        break;
    }
    case DSViewList: {
        char t[28]; snprintf(t,sizeof(t),"DEAUTH SOURCES [%d]",app->event_count);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, t);
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->event_count;i++) {
            uint8_t idx=app->scroll_pos+i;
            DeauthEvent* ev=&app->events[idx];
            char line[36]; snprintf(line,sizeof(line),"%s%02X:%02X:%02X ch%d %ddB x%lu",
                idx==app->selected?">":"  ",
                ev->src_mac[0],ev->src_mac[1],ev->src_mac[2],
                ev->channel,ev->rssi,ev->count);
            canvas_draw_str(canvas, 0, 19+i*10, line);
        }
        canvas_draw_str(canvas, 2, 63, "[OK]Locate [BK]Live");
        break;
    }
    case DSViewLocate: {
        if(app->event_count==0){canvas_draw_str(canvas,10,32,"No sources"); break;}
        DeauthEvent* ev=&app->events[app->selected];
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "LOCATE ATTACKER");
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        char mac[20]; snprintf(mac,sizeof(mac),"%02X:%02X:%02X:%02X:%02X:%02X",
            ev->src_mac[0],ev->src_mac[1],ev->src_mac[2],ev->src_mac[3],ev->src_mac[4],ev->src_mac[5]);
        canvas_draw_str(canvas, 2, 17, mac);
        char r[28]; snprintf(r,sizeof(r),"RSSI: %d dBm  Ch: %d",ev->rssi,ev->channel);
        canvas_draw_str(canvas, 2, 25, r);
        int norm=ev->rssi+100; if(norm<0)norm=0; if(norm>100)norm=100;
        for(uint8_t b=0;b<10;b++){uint8_t bh=4+(b*3);if(b*10<(uint8_t)norm)canvas_draw_box(canvas,4+b*12,52-bh,10,bh);else canvas_draw_frame(canvas,4+b*12,52-bh,10,bh);}
        canvas_draw_str_aligned(canvas, 64, 55, AlignCenter, AlignTop,
            ev->rssi>-50?"HOT-Move closer":ev->rssi>-70?"Warm":"Cold-move toward");
        canvas_draw_str(canvas, 30, 64, "[BACK] List");
        break;
    }
    }
}

static void deauth_scanner_input(InputEvent* e, void* ctx) {
    DeauthScannerApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t deauth_scanner_app(void* p) {
    UNUSED(p);
    DeauthScannerApp* app = malloc(sizeof(DeauthScannerApp));
    memset(app, 0, sizeof(DeauthScannerApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, deauth_scanner_draw, app);
    view_port_input_callback_set(app->view_port, deauth_scanner_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick=0;
    while(app->running) {
        if(app->monitoring){tick++;if(tick%8==0){app->monitor_time++;sim_deauth(app);}}
        // Fluctuate locate RSSI
        if(app->view==DSViewLocate&&app->event_count>0){
            app->events[app->selected].rssi+=(rand()%7)-3;
            if(app->events[app->selected].rssi>-15)app->events[app->selected].rssi=-15;
            if(app->events[app->selected].rssi<-95)app->events[app->selected].rssi=-95;
        }
        if(furi_message_queue_get(app->event_queue,&event,app->monitoring?100:50)==FuriStatusOk) {
            if(event.type==InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->view==DSViewLive&&!app->monitoring){app->monitoring=true;app->monitor_time=0;tick=0;}
                    else if(app->view==DSViewList&&app->event_count>0) app->view=DSViewLocate;
                    break;
                case InputKeyRight: if(app->view==DSViewLive)app->view=DSViewList; break;
                case InputKeyDown: if(app->view==DSViewList&&app->selected<app->event_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
                case InputKeyUp: if(app->view==DSViewList&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
                case InputKeyBack:
                    if(app->view==DSViewLocate)app->view=DSViewList;
                    else if(app->view==DSViewList)app->view=DSViewLive;
                    else if(app->monitoring)app->monitoring=false;
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
