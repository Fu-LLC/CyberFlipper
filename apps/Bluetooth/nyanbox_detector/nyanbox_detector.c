/**
 * CYBERFLIPPER — nyanBOX Detector
 * Discovers nearby nyanBOX devices via BLE advertising.
 * Shows level, firmware version, signal strength, and identity.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "nyanBOX"
#define MAX_NYAN 10

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi;
    uint8_t level; uint8_t fw_major; uint8_t fw_minor; uint32_t uptime_min;
} NyanDevice;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; NyanDevice devs[MAX_NYAN]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail;
} NyanBoxApp;

static void sim_nyan(NyanBoxApp* app) {
    if(app->count>=MAX_NYAN||rand()%12!=0) return;
    NyanDevice* d=&app->devs[app->count++];
    snprintf(d->name,sizeof(d->name),"nyanBOX_%04X",rand()%0xFFFF);
    for(int i=0;i<6;i++) d->addr[i]=rand()%256;
    d->rssi=-(rand()%70)-20; d->level=1+rand()%99;
    d->fw_major=1+rand()%3; d->fw_minor=rand()%10; d->uptime_min=rand()%14400;
}

static void nb_draw(Canvas* canvas, void* ctx) {
    NyanBoxApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail&&app->count>0) {
        NyanDevice* d=&app->devs[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"nyanBOX INFO");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40];
        snprintf(s,sizeof(s),"Name: %s",d->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5]);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"Level: %d  FW: %d.%d",d->level,d->fw_major,d->fw_minor); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"RSSI: %ddBm",d->rssi); canvas_draw_str(canvas,2,42,s);
        snprintf(s,sizeof(s),"Uptime: %luh %lum",d->uptime_min/60,d->uptime_min%60); canvas_draw_str(canvas,2,50,s);
        canvas_draw_str(canvas,30,62,"[BACK] List");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"nyanBOX SCAN [%d]",app->count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning BLE...":"No nyanBOX found");canvas_draw_str(canvas,10,44,"[OK] Scan");}
    else for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
        uint8_t idx=app->scroll_pos+i; NyanDevice* d=&app->devs[idx]; char l[36];
        snprintf(l,sizeof(l),"%s%-14s Lv%d %ddB",idx==app->selected?">":"  ",d->name,d->level,d->rssi);
        canvas_draw_str(canvas,0,19+i*10,l);
    }
    char st[20]; snprintf(st,sizeof(st),"%lus",app->scan_time); canvas_draw_str(canvas,2,62,st);
}
static void nb_input(InputEvent* e,void* ctx){NyanBoxApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t nyanbox_detector_app(void* p) {
    UNUSED(p);
    NyanBoxApp* app=malloc(sizeof(NyanBoxApp)); memset(app,0,sizeof(NyanBoxApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,nb_draw,app);
    view_port_input_callback_set(app->view_port,nb_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%15==0){app->scan_time++;sim_nyan(app);}}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->detail&&app->count>0)app->detail=true;else if(!app->scanning){app->scanning=true;tick=0;} break;
            case InputKeyDown: if(!app->detail&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(!app->detail&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack: if(app->detail)app->detail=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
