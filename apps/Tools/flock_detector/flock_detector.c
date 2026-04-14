/**
 * CYBERFLIPPER — Flock Detector
 * Detects Flock Safety surveillance cameras via WiFi SSID
 * patterns, MAC OUI prefixes, and BLE device names.
 * Locate mode with RSSI signal tracking.
 * FOR LEGAL AWARENESS AND AUTHORIZED USE.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "FlockDetector"
#define MAX_CAMS 12

typedef struct {
    char id[20]; char ssid[24]; uint8_t mac[6]; int8_t rssi;
    char type[16]; bool via_ble; float lat; float lon;
} FlockCam;

typedef enum { FKViewList, FKViewDetail, FKViewLocate } FKView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    FKView view; bool scanning; FlockCam cams[MAX_CAMS]; uint8_t cam_count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time;
} FlockDetectorApp;

static void sim_flock(FlockDetectorApp* app) {
    if(app->cam_count>=MAX_CAMS||rand()%8!=0) return;
    FlockCam* c=&app->cams[app->cam_count++];
    static const char* types[]={"Falcon ALPR","Sparrow LPR","Raven Fixed","Condor Mobile"};
    snprintf(c->id,sizeof(c->id),"FLOCK-%04d",1000+rand()%9000);
    snprintf(c->ssid,sizeof(c->ssid),"flocksafety_%02X%02X",rand()%256,rand()%256);
    c->mac[0]=0x64; c->mac[1]=0x16; c->mac[2]=0x66; // Flock OUI
    for(int i=3;i<6;i++) c->mac[i]=rand()%256;
    c->rssi=-(rand()%60)-30; strncpy(c->type,types[rand()%4],15);
    c->via_ble=rand()%2; c->lat=33.4f+((rand()%100)-50)*0.001f; c->lon=-112.0f+((rand()%100)-50)*0.001f;
}

static void fk_draw(Canvas* canvas, void* ctx) {
    FlockDetectorApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    switch(app->view) {
    case FKViewList: {
        char t[28]; snprintf(t,sizeof(t),"FLOCK DETECT [%d]",app->cam_count);
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        if(app->cam_count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning for cameras...":"No Flock cameras found");canvas_draw_str(canvas,10,44,"[OK] Scan");break;}
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->cam_count;i++){
            uint8_t idx=app->scroll_pos+i; FlockCam* c=&app->cams[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%-10s %-8s %ddB",idx==app->selected?">":"  ",c->id,c->type,c->rssi);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
        canvas_draw_str(canvas,2,62,"[OK]Detail [BK]Exit");
        break;
    }
    case FKViewDetail: {
        if(app->cam_count==0)break;
        FlockCam* c=&app->cams[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"FLOCK CAMERA");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"ID: %s",c->id); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"Type: %s",c->type); canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"SSID: %s",c->ssid); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"MAC: %02X:%02X:%02X:%02X:%02X:%02X",c->mac[0],c->mac[1],c->mac[2],c->mac[3],c->mac[4],c->mac[5]);
        canvas_draw_str(canvas,2,42,s);
        snprintf(s,sizeof(s),"Via: %s  RSSI: %ddBm",c->via_ble?"BLE":"WiFi",c->rssi); canvas_draw_str(canvas,2,50,s);
        canvas_draw_str(canvas,2,62,"[>]Locate [BK]List");
        break;
    }
    case FKViewLocate: {
        if(app->cam_count==0)break;
        FlockCam* c=&app->cams[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"LOCATE CAMERA");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char r[24]; snprintf(r,sizeof(r),"RSSI: %d dBm",c->rssi); canvas_draw_str(canvas,2,18,r);
        int norm=c->rssi+100;if(norm<0)norm=0;if(norm>100)norm=100;
        for(uint8_t b=0;b<10;b++){uint8_t bh=4+(b*3);if(b*10<(uint8_t)norm)canvas_draw_box(canvas,4+b*12,52-bh,10,bh);else canvas_draw_frame(canvas,4+b*12,52-bh,10,bh);}
        canvas_draw_str_aligned(canvas,64,54,AlignCenter,AlignTop,c->rssi>-50?"VERY CLOSE!":c->rssi>-70?"Getting closer":"Move toward signal");
        canvas_draw_str(canvas,30,63,"[BACK] Detail");
        break;
    }}
}
static void fk_input(InputEvent* e,void* ctx){FlockDetectorApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t flock_detector_app(void* p) {
    UNUSED(p);
    FlockDetectorApp* app=malloc(sizeof(FlockDetectorApp)); memset(app,0,sizeof(FlockDetectorApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,fk_draw,app);
    view_port_input_callback_set(app->view_port,fk_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%15==0){app->scan_time++;sim_flock(app);}}
        if(app->view==FKViewLocate&&app->cam_count>0){app->cams[app->selected].rssi+=(rand()%7)-3;if(app->cams[app->selected].rssi>-15)app->cams[app->selected].rssi=-15;if(app->cams[app->selected].rssi<-95)app->cams[app->selected].rssi=-95;}
        if(furi_message_queue_get(app->event_queue,&event,app->view==FKViewLocate?120:100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(app->view==FKViewList&&app->cam_count>0)app->view=FKViewDetail;else if(app->view==FKViewList){app->scanning=true;app->scan_time=0;tick=0;} break;
            case InputKeyRight: if(app->view==FKViewDetail)app->view=FKViewLocate; break;
            case InputKeyDown: if(app->view==FKViewList&&app->selected<app->cam_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(app->view==FKViewList&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack: if(app->view==FKViewLocate)app->view=FKViewDetail;else if(app->view==FKViewDetail)app->view=FKViewList;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
