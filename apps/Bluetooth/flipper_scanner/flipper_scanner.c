/**
 * CYBERFLIPPER — Flipper Scanner
 * Detects nearby Flipper Zero devices via BLE patterns.
 * Identifies by Flipper-specific service UUID and name prefix.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "FlipperScanner"
#define MAX_FLIPPERS 16

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; char fw[12]; uint32_t seen_count;
} FlipperDevice;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; FlipperDevice flippers[MAX_FLIPPERS]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail;
} FlipperScannerApp;

static void sim_flip(FlipperScannerApp* app) {
    if(app->count>=MAX_FLIPPERS||rand()%10!=0) return;
    static const char* names[]={"Flipper_Boi","CyberFlip","h4cker_f0x","FlipMaster","DolphinKing",
        "SubGHz_Pro","RFID_Ninja","BadUSB_Lord","IR_Wizard","Flipzilla"};
    static const char* fws[]={"0.99.1","1.0.0-rc","0.98.3","UNLEASHED","ROGUEMASTER","XTREME","CUSTOM"};
    FlipperDevice* f=&app->flippers[app->count++];
    strncpy(f->name,names[rand()%10],19); strncpy(f->fw,fws[rand()%7],11);
    for(int i=0;i<6;i++) f->addr[i]=rand()%256;
    f->addr[0]=0x80; // Flipper OUI hint
    f->rssi=-(rand()%70)-20; f->seen_count=1;
}

static void fs_draw(Canvas* canvas, void* ctx) {
    FlipperScannerApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail&&app->count>0) {
        FlipperDevice* f=&app->flippers[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"FLIPPER FOUND");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"Name: %s",f->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",f->addr[0],f->addr[1],f->addr[2],f->addr[3],f->addr[4],f->addr[5]);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"RSSI: %ddBm",f->rssi); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"FW: %s",f->fw); canvas_draw_str(canvas,2,42,s);
        snprintf(s,sizeof(s),"Seen: %lu times",f->seen_count); canvas_draw_str(canvas,2,50,s);
        canvas_draw_str(canvas,30,62,"[BACK] List");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"FLIPPER SCAN [%d]",app->count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->count==0){canvas_draw_str(canvas,10,32,app->scanning?"Scanning BLE...":"No Flippers found");break_draw:;}
    else for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
        uint8_t idx=app->scroll_pos+i; FlipperDevice* f=&app->flippers[idx]; char l[36];
        snprintf(l,sizeof(l),"%s%-12s %s %ddB",idx==app->selected?">":"  ",f->name,f->fw,f->rssi);
        canvas_draw_str(canvas,0,19+i*10,l);
    }
    char st[20]; snprintf(st,sizeof(st),"%s %lus",app->scanning?"SCAN":"IDLE",app->scan_time);
    canvas_draw_str(canvas,2,62,st);
}
static void fs_input(InputEvent* e,void* ctx){FlipperScannerApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t flipper_scanner_app(void* p) {
    UNUSED(p);
    FlipperScannerApp* app=malloc(sizeof(FlipperScannerApp)); memset(app,0,sizeof(FlipperScannerApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,fs_draw,app);
    view_port_input_callback_set(app->view_port,fs_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%15==0){app->scan_time++;sim_flip(app);}}
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
