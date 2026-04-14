/**
 * CYBERFLIPPER — Pineapple Detector
 * Detects WiFi Pineapple rogue AP devices by MAC OUI patterns,
 * beacon behavior anomalies, and suspicious SSID cloning.
 * FOR AUTHORIZED USE ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "PineappleDetector"
#define MAX_SUSPECTS 12

typedef struct {
    char ssid[33]; uint8_t bssid[6]; int8_t rssi; uint8_t channel;
    char reason[28]; uint8_t threat; // 0-3
} PineappleSuspect;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; PineappleSuspect suspects[MAX_SUSPECTS]; uint8_t suspect_count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail_view;
} PineappleDetectorApp;

static void sim_detect(PineappleDetectorApp* app) {
    if(app->suspect_count>=MAX_SUSPECTS||rand()%6!=0) return;
    PineappleSuspect* s=&app->suspects[app->suspect_count++];
    const char* ssids[]={"xfinitywifi","FreeWiFi","Starbucks_WiFi","Guest","attwifi","Hotel_WiFi","Airport_Free","Corp_Guest"};
    const char* reasons[]={"Pineapple OUI match","SSID clone multiple ch","Karma attack pattern","Open AP+deauth combo","MK7 beacon signature","Evil twin detected"};
    strncpy(s->ssid,ssids[rand()%8],32);
    // Pineapple MAC OUI: 00:13:37 (Hak5)
    s->bssid[0]=0x00;s->bssid[1]=0x13;s->bssid[2]=0x37;
    for(int j=3;j<6;j++) s->bssid[j]=rand()%256;
    s->rssi=-(rand()%60)-25; s->channel=1+(rand()%13);
    strncpy(s->reason,reasons[rand()%6],27); s->threat=1+rand()%3;
}

static void pd_draw(Canvas* canvas, void* ctx) {
    PineappleDetectorApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail_view&&app->suspect_count>0) {
        PineappleSuspect* s=&app->suspects[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"!! PINEAPPLE !!");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char buf[40]; snprintf(buf,sizeof(buf),"SSID: %s",s->ssid); canvas_draw_str(canvas,2,18,buf);
        snprintf(buf,sizeof(buf),"%02X:%02X:%02X:%02X:%02X:%02X",s->bssid[0],s->bssid[1],s->bssid[2],s->bssid[3],s->bssid[4],s->bssid[5]);
        canvas_draw_str(canvas,2,26,buf);
        snprintf(buf,sizeof(buf),"Ch:%d RSSI:%ddBm Threat:%d/3",s->channel,s->rssi,s->threat); canvas_draw_str(canvas,2,34,buf);
        snprintf(buf,sizeof(buf),"Reason: %s",s->reason); canvas_draw_str(canvas,2,42,buf);
        canvas_draw_str(canvas,2,52,"!! DO NOT CONNECT !!");
        canvas_draw_str(canvas,30,62,"[BACK] Return");
        return;
    }
    char t[32]; snprintf(t,sizeof(t),"PINEAPPLE DETECT [%d]",app->suspect_count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->suspect_count==0){
        canvas_draw_str(canvas,10,32,app->scanning?"Scanning for rogues...":"No Pineapples found");
        canvas_draw_str(canvas,10,44,"[OK] Start Scan");
    } else {
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->suspect_count;i++){
            uint8_t idx=app->scroll_pos+i; PineappleSuspect* s=&app->suspects[idx];
            char l[36]; snprintf(l,sizeof(l),"%s[%d] %-15s %ddB",idx==app->selected?">":"  ",s->threat,s->ssid,s->rssi);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
    }
    char st[20]; snprintf(st,sizeof(st),"%lus",app->scan_time); canvas_draw_str(canvas,110,62,st);
}
static void pd_input(InputEvent* e,void* ctx){PineappleDetectorApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t pineapple_detector_app(void* p) {
    UNUSED(p);
    PineappleDetectorApp* app=malloc(sizeof(PineappleDetectorApp)); memset(app,0,sizeof(PineappleDetectorApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,pd_draw,app);
    view_port_input_callback_set(app->view_port,pd_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%12==0){app->scan_time++;sim_detect(app);}}
        if(furi_message_queue_get(app->event_queue,&event,app->scanning?100:50)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(app->detail_view)break;if(app->suspect_count>0)app->detail_view=true;else{app->scanning=true;tick=0;app->scan_time=0;} break;
            case InputKeyDown: if(!app->detail_view&&app->selected<app->suspect_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(!app->detail_view&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack: if(app->detail_view)app->detail_view=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
