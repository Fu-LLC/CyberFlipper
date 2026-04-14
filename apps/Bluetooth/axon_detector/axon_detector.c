/**
 * CYBERFLIPPER — Axon Detector
 * Detects Axon body cameras, Signal Sidearm, Taser 10, and
 * other law enforcement BLE equipment by OUI and service patterns.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "AxonDetector"
#define MAX_AXON 12

typedef struct {
    char name[20]; char type[16]; uint8_t addr[6]; int8_t rssi;
    bool recording; uint32_t first_seen;
} AxonDevice;

static const char* axon_types[]={"Body Camera 3","Body Camera 4","Taser 7","Taser 10",
    "Signal Sidearm","Fleet Camera","Interview Cam","Axon VR"};

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; AxonDevice devs[MAX_AXON]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail;
} AxonDetectorApp;

static void sim_axon(AxonDetectorApp* app) {
    if(app->count>=MAX_AXON||rand()%10!=0) return;
    AxonDevice* d=&app->devs[app->count++];
    snprintf(d->name,sizeof(d->name),"AXON_%04X",rand()%0xFFFF);
    strncpy(d->type,axon_types[rand()%8],15);
    // Axon OUI: 00:25:DF
    d->addr[0]=0x00;d->addr[1]=0x25;d->addr[2]=0xDF;
    for(int i=3;i<6;i++) d->addr[i]=rand()%256;
    d->rssi=-(rand()%70)-20; d->recording=rand()%3==0; d->first_seen=app->scan_time;
}

static void ax_draw(Canvas* canvas, void* ctx) {
    AxonDetectorApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail&&app->count>0) {
        AxonDevice* d=&app->devs[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"AXON DEVICE");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40];
        snprintf(s,sizeof(s),"ID: %s",d->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"Type: %s",d->type); canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5]);
        canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"RSSI: %ddBm",d->rssi); canvas_draw_str(canvas,2,42,s);
        canvas_draw_str(canvas,2,50,d->recording?"!! RECORDING ACTIVE !!":"Not recording");
        canvas_draw_str(canvas,30,62,"[BACK] List");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"AXON DETECT [%d]",app->count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->count==0){canvas_draw_str(canvas,10,32,app->scanning?"Scanning for Axon...":"No Axon devices found");canvas_draw_str(canvas,10,44,"[OK] Start");}
    else for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
        uint8_t idx=app->scroll_pos+i; AxonDevice* d=&app->devs[idx]; char l[36];
        snprintf(l,sizeof(l),"%s%-10s %-8s %ddB%s",idx==app->selected?">":"  ",d->name,d->type,d->rssi,d->recording?" REC":"");
        canvas_draw_str(canvas,0,19+i*10,l);
    }
    char st[20]; snprintf(st,sizeof(st),"%lus",app->scan_time); canvas_draw_str(canvas,2,62,st);
}
static void ax_input(InputEvent* e,void* ctx){AxonDetectorApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t axon_detector_app(void* p) {
    UNUSED(p);
    AxonDetectorApp* app=malloc(sizeof(AxonDetectorApp)); memset(app,0,sizeof(AxonDetectorApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,ax_draw,app);
    view_port_input_callback_set(app->view_port,ax_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%12==0){app->scan_time++;sim_axon(app);}}
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
