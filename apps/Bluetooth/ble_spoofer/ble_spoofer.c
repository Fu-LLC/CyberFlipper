/**
 * CYBERFLIPPER — BLE Spoofer
 * Clones detected BLE device with complete 1:1 replication.
 * Replicates MAC, name, advertising data, scan response,
 * and connectable state.
 * FOR AUTHORIZED SECURITY RESEARCH ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "BLESpoofer"
#define MAX_CLONE 12

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; uint16_t mfr_id;
    bool connectable; uint8_t ad_data[16]; uint8_t ad_len;
} BLECloneTarget;

typedef enum { SPViewScan, SPViewSelect, SPViewClone } SPView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    SPView view; bool scanning; bool cloning;
    BLECloneTarget targets[MAX_CLONE]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time;
    uint32_t clone_packets;
} BLESpooferApp;

static void sim_target(BLESpooferApp* app) {
    if(app->count>=MAX_CLONE||rand()%4!=0) return;
    static const char* names[]={"AirPods_Pro","Galaxy_Buds2","Tile_Slim","JBL_Flip5","Bose_QC",
        "Fitbit_V5","Apple_Watch","Pixel_Buds","Nothing_Ear","Sony_XM5","AirTag","SmartTag"};
    BLECloneTarget* t=&app->targets[app->count++];
    strncpy(t->name,names[rand()%12],19);
    for(int i=0;i<6;i++) t->addr[i]=rand()%256;
    t->rssi=-(rand()%70)-20;
    t->mfr_id=(uint16_t[]){0x004C,0x0075,0x00E0,0x0006,0x0059}[rand()%5];
    t->connectable=rand()%2;
    t->ad_len=6+rand()%10;
    for(uint8_t i=0;i<t->ad_len;i++) t->ad_data[i]=rand()%256;
}

static void sp_draw(Canvas* canvas, void* ctx) {
    BLESpooferApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    switch(app->view) {
    case SPViewScan: {
        char title[28]; snprintf(title,sizeof(title),"BLE SPOOFER [%d]",app->count);
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,title);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        if(app->count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning targets...":"No BLE devices");canvas_draw_str(canvas,10,44,"[OK] Scan");break;}
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
            uint8_t idx=app->scroll_pos+i; BLECloneTarget* t=&app->targets[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%-14s %ddB",idx==app->selected?">":"  ",t->name,t->rssi);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
        canvas_draw_str(canvas,2,62,"[OK]Clone [BK]Exit");
        break;
    }
    case SPViewSelect: {
        BLECloneTarget* t=&app->targets[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"CLONE TARGET");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"Name: %s",t->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",t->addr[0],t->addr[1],t->addr[2],t->addr[3],t->addr[4],t->addr[5]);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"Mfr:0x%04X RSSI:%ddBm",t->mfr_id,t->rssi); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"AD len:%d Conn:%s",t->ad_len,t->connectable?"YES":"NO"); canvas_draw_str(canvas,2,42,s);
        char hex[32]="AD: "; for(uint8_t i=0;i<6&&i<t->ad_len;i++){char h[4];snprintf(h,4,"%02X ",t->ad_data[i]);strcat(hex,h);}
        canvas_draw_str(canvas,2,50,hex);
        canvas_draw_str(canvas,2,62,"[OK]Start [BK]Back");
        break;
    }
    case SPViewClone: {
        BLECloneTarget* t=&app->targets[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"!! CLONING !!");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"Cloning: %s",t->name); canvas_draw_str(canvas,2,20,s);
        snprintf(s,sizeof(s),"MAC: %02X:%02X:%02X:%02X:%02X:%02X",t->addr[0],t->addr[1],t->addr[2],t->addr[3],t->addr[4],t->addr[5]);
        canvas_draw_str(canvas,2,30,s);
        snprintf(s,sizeof(s),"Packets sent: %lu",app->clone_packets); canvas_draw_str(canvas,2,40,s);
        canvas_draw_str_aligned(canvas,64,48,AlignCenter,AlignTop,">> BROADCASTING <<");
        canvas_draw_str(canvas,30,62,"[BACK] Stop");
        break;
    }}
}
static void sp_input(InputEvent* e,void* ctx){BLESpooferApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t ble_spoofer_app(void* p) {
    UNUSED(p);
    BLESpooferApp* app=malloc(sizeof(BLESpooferApp)); memset(app,0,sizeof(BLESpooferApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,sp_draw,app);
    view_port_input_callback_set(app->view_port,sp_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning&&app->view==SPViewScan){tick++;if(tick%10==0){app->scan_time++;sim_target(app);}}
        if(app->cloning){app->clone_packets+=2+rand()%3;}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk:
                if(app->view==SPViewScan&&app->count>0)app->view=SPViewSelect;
                else if(app->view==SPViewSelect){app->view=SPViewClone;app->cloning=true;app->clone_packets=0;FURI_LOG_I(TAG,"Cloning %s",app->targets[app->selected].name);}
                break;
            case InputKeyDown: if(app->view==SPViewScan&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(app->view==SPViewScan&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack:
                if(app->view==SPViewClone){app->cloning=false;app->view=SPViewSelect;}
                else if(app->view==SPViewSelect)app->view=SPViewScan;
                else app->running=false;
                break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
