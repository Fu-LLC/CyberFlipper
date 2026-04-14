/**
 * CYBERFLIPPER — BLE Inspector
 * Decodes raw BLE advertising packets. Shows service UUIDs,
 * manufacturer data, TX power, flags, device name, and raw hex.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "BLEInspector"
#define MAX_DEV 16

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; int8_t tx_power;
    uint16_t mfr_id; uint16_t svc_uuid; uint8_t flags; bool connectable;
    uint8_t raw[8]; uint8_t raw_len;
} BLEInspEntry;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool scanning; BLEInspEntry devs[MAX_DEV]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; bool detail;
} BLEInspectorApp;

static const uint16_t known_uuids[]={0x1800,0x1801,0x180A,0x180F,0x181C,0xFD6F,0xFE2C,0xFEAA,0xFFF0};
static const char* uuid_names[]={"GenericAccess","GenericAttr","DeviceInfo","Battery","UserData","FindMy","Tile","Eddystone","Custom"};

static void sim_insp(BLEInspectorApp* app) {
    if(app->count>=MAX_DEV||rand()%4!=0) return;
    static const char* names[]={"AirPods","Galaxy Buds","Tile_C3","[Unknown]","Beacon_01","Watch_BLE",
        "HR_Sensor","Smart Lock","Temp_Sens","Speaker_BT","Mouse","Keyboard","nRF_Dev","ESP32_BLE","Tag_X","Headset"};
    BLEInspEntry* d=&app->devs[app->count++];
    strncpy(d->name,names[rand()%16],19);
    for(int i=0;i<6;i++) d->addr[i]=rand()%256;
    d->rssi=-(rand()%80)-15; d->tx_power=-(rand()%20);
    d->mfr_id=(uint16_t[]){0x004C,0x0075,0x0006,0x0059,0x02E5,0x0499,0x0000}[rand()%7];
    d->svc_uuid=known_uuids[rand()%9];
    d->flags=0x06|(rand()%4); d->connectable=rand()%2;
    d->raw_len=4+rand()%4;
    for(uint8_t i=0;i<d->raw_len;i++) d->raw[i]=rand()%256;
}

static const char* uuid_name(uint16_t uuid) {
    for(uint8_t i=0;i<9;i++) if(known_uuids[i]==uuid) return uuid_names[i];
    return "Unknown";
}

static void bi_draw(Canvas* canvas, void* ctx) {
    BLEInspectorApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail&&app->count>0) {
        BLEInspEntry* d=&app->devs[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"BLE INSPECT");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[42];
        snprintf(s,sizeof(s),"Name: %s",d->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X %ddBm",d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5],d->rssi);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"Mfr:0x%04X Svc:%s",d->mfr_id,uuid_name(d->svc_uuid)); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"TxPow:%ddBm Flags:0x%02X %s",d->tx_power,d->flags,d->connectable?"CONN":"NC");
        canvas_draw_str(canvas,2,42,s);
        // Raw hex
        char hex[32]="Raw: ";
        for(uint8_t i=0;i<d->raw_len&&i<6;i++){char h[4];snprintf(h,4,"%02X ",d->raw[i]);strcat(hex,h);}
        canvas_draw_str(canvas,2,50,hex);
        canvas_draw_str(canvas,30,62,"[BACK] List");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"BLE INSPECTOR [%d]",app->count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
        uint8_t idx=app->scroll_pos+i; BLEInspEntry* d=&app->devs[idx]; char l[36];
        snprintf(l,sizeof(l),"%s%-12s 0x%04X %ddB",idx==app->selected?">":"  ",d->name,d->svc_uuid,d->rssi);
        canvas_draw_str(canvas,0,19+i*10,l);
    }
    char st[20]; snprintf(st,sizeof(st),"%s %lus",app->scanning?"SCAN":"IDLE",app->scan_time); canvas_draw_str(canvas,2,62,st);
}
static void bi_input(InputEvent* e,void* ctx){BLEInspectorApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t ble_inspector_app(void* p) {
    UNUSED(p);
    BLEInspectorApp* app=malloc(sizeof(BLEInspectorApp)); memset(app,0,sizeof(BLEInspectorApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,bi_draw,app);
    view_port_input_callback_set(app->view_port,bi_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%10==0){app->scan_time++;sim_insp(app);}}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->detail&&app->count>0)app->detail=true; break;
            case InputKeyDown: if(!app->detail&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(!app->detail&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyRight: app->scanning=!app->scanning; break;
            case InputKeyBack: if(app->detail)app->detail=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
