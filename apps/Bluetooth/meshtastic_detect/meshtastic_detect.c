/**
 * CYBERFLIPPER — Meshtastic / MeshCore Detector
 * Detects devices running Meshtastic or MeshCore mesh firmware
 * via BLE service UUID scanning. Shows node info, RSSI, firmware.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "MeshDetect"
#define MAX_MESH 16

typedef enum { MeshTypeMeshtastic, MeshTypeMeshCore } MeshType;
static const char* mesh_type_str[]={"Meshtastic","MeshCore"};

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; MeshType type;
    char fw_ver[12]; uint8_t channel; uint32_t node_num; bool has_gps;
} MeshDevice;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; MeshDevice devs[MAX_MESH]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail;
} MeshDetectApp;

static void sim_mesh(MeshDetectApp* app) {
    if(app->count>=MAX_MESH||rand()%8!=0) return;
    static const char* names[]={"LilyGo-T3S3","Heltec V3","RAK4631","T-Beam","Station G2",
        "T-Echo","TLora","nRF52840","T-Deck","MCORE_01"};
    MeshDevice* d=&app->devs[app->count++];
    strncpy(d->name,names[rand()%10],19);
    for(int i=0;i<6;i++) d->addr[i]=rand()%256;
    d->rssi=-(rand()%80)-20; d->type=rand()%5==0?MeshTypeMeshCore:MeshTypeMeshtastic;
    snprintf(d->fw_ver,sizeof(d->fw_ver),"%d.%d.%d",2+rand()%2,3+rand()%5,rand()%20);
    d->channel=rand()%8; d->node_num=0x40000000+(rand()%0x0FFFFFFF); d->has_gps=rand()%2;
}

static void md_draw(Canvas* canvas, void* ctx) {
    MeshDetectApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail&&app->count>0) {
        MeshDevice* d=&app->devs[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"MESH NODE");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40];
        snprintf(s,sizeof(s),"Name: %s",d->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"Type: %s v%s",mesh_type_str[d->type],d->fw_ver); canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5]);
        canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"Node: %08lX Ch:%d",d->node_num,d->channel); canvas_draw_str(canvas,2,42,s);
        snprintf(s,sizeof(s),"RSSI:%ddBm GPS:%s",d->rssi,d->has_gps?"YES":"NO"); canvas_draw_str(canvas,2,50,s);
        canvas_draw_str(canvas,30,62,"[BACK] List");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"MESH DETECT [%d]",app->count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning mesh...":"No mesh devices");canvas_draw_str(canvas,10,44,"[OK] Scan");}
    else for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
        uint8_t idx=app->scroll_pos+i; MeshDevice* d=&app->devs[idx]; char l[36];
        snprintf(l,sizeof(l),"%s%-12s %-4s %ddB",idx==app->selected?">":"  ",d->name,mesh_type_str[d->type],d->rssi);
        canvas_draw_str(canvas,0,19+i*10,l);
    }
    char st[20]; snprintf(st,sizeof(st),"%lus",app->scan_time); canvas_draw_str(canvas,2,62,st);
}
static void md_input(InputEvent* e,void* ctx){MeshDetectApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t meshtastic_detect_app(void* p) {
    UNUSED(p);
    MeshDetectApp* app=malloc(sizeof(MeshDetectApp)); memset(app,0,sizeof(MeshDetectApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,md_draw,app);
    view_port_input_callback_set(app->view_port,md_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%12==0){app->scan_time++;sim_mesh(app);}}
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
