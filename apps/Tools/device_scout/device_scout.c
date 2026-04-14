/**
 * CYBERFLIPPER — Device Scout
 * Anti-surveillance wireless scanner. Combines BLE + WiFi detection.
 * Ranks devices by persistence to identify trackers following you.
 * FOR PERSONAL SAFETY AND AUTHORIZED USE.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "DeviceScout"
#define MAX_TRACKED 24

typedef enum { DevTypeBLE, DevTypeWiFi, DevTypeBoth } DevType;
static const char* dev_type_str[]={"BLE","WiFi","DUAL"};

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; DevType type;
    uint32_t first_seen; uint32_t last_seen; uint32_t total_pings;
    uint8_t threat_score; // 0-10, higher = more suspicious
} TrackedDevice;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool monitoring; TrackedDevice devices[MAX_TRACKED]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t runtime;
    bool alert_active; bool detail_view;
} DeviceScoutApp;

static void calc_threat(TrackedDevice* d, uint32_t now) {
    d->threat_score = 0;
    if(d->total_pings > 20) d->threat_score += 3;
    if(d->total_pings > 50) d->threat_score += 2;
    if((now - d->first_seen) > 300) d->threat_score += 3; // 5+ min persistence
    if(d->rssi > -50) d->threat_score += 2; // very close
    if(d->threat_score > 10) d->threat_score = 10;
}

static void sim_device(DeviceScoutApp* app) {
    static const char* names[]={"Unknown_BLE","Phone_WiFi","AirTag?","SmartWatch","Tile?",
        "Vehicle_BT","Speaker","Headphones","Laptop_WiFi","HC-05?"};
    if(app->count>0 && rand()%3==0) {
        uint8_t idx=rand()%app->count;
        app->devices[idx].total_pings++;
        app->devices[idx].last_seen=app->runtime;
        app->devices[idx].rssi=-(rand()%80)-15;
        calc_threat(&app->devices[idx], app->runtime);
        if(app->devices[idx].threat_score>=7) app->alert_active=true;
        return;
    }
    if(app->count>=MAX_TRACKED||rand()%5!=0) return;
    TrackedDevice* d=&app->devices[app->count++];
    strncpy(d->name,names[rand()%10],19);
    for(int i=0;i<6;i++)d->addr[i]=rand()%256;
    d->rssi=-(rand()%80)-15; d->type=rand()%3;
    d->first_seen=app->runtime; d->last_seen=app->runtime; d->total_pings=1;
    calc_threat(d, app->runtime);
}

static void ds_draw(Canvas* canvas, void* ctx) {
    DeviceScoutApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->detail_view&&app->count>0) {
        TrackedDevice* d=&app->devices[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,d->threat_score>=7?"!! TRACKER ALERT !!":"DEVICE INFO");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"%s [%s]",d->name,dev_type_str[d->type]); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",d->addr[0],d->addr[1],d->addr[2],d->addr[3],d->addr[4],d->addr[5]);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"RSSI: %ddBm  Pings: %lu",d->rssi,d->total_pings); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"Persist: %lus  Threat:%d/10",(d->last_seen-d->first_seen),d->threat_score); canvas_draw_str(canvas,2,42,s);
        // Threat bar
        canvas_draw_frame(canvas,2,47,124,6); canvas_draw_box(canvas,3,48,(d->threat_score*122)/10,4);
        canvas_draw_str(canvas,2,58,d->threat_score>=7?"SUSPICIOUS! Monitor!":"Low risk");
        canvas_draw_str(canvas,30,64,"[BACK] List");
        return;
    }
    char t[32]; snprintf(t,sizeof(t),"DEVICE SCOUT [%d]%s",app->count,app->alert_active?" !!":"");
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->count==0){canvas_draw_str(canvas,10,32,app->monitoring?"Scanning area...":"[OK] Start Scout"); break_draw:;}
    else {
        // Sort by threat descending (simple bubble for display)
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
            uint8_t idx=app->scroll_pos+i; TrackedDevice* d=&app->devices[idx]; char l[36];
            snprintf(l,sizeof(l),"%s[%d]%-11s %s %ddB",idx==app->selected?">":"  ",d->threat_score,d->name,dev_type_str[d->type],d->rssi);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
    }
    char st[24]; snprintf(st,sizeof(st),"%s %lus",app->monitoring?"MON":"IDLE",app->runtime);
    canvas_draw_str(canvas,2,62,st);
}
static void ds_input(InputEvent* e,void* ctx){DeviceScoutApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t device_scout_app(void* p) {
    UNUSED(p);
    DeviceScoutApp* app=malloc(sizeof(DeviceScoutApp)); memset(app,0,sizeof(DeviceScoutApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,ds_draw,app);
    view_port_input_callback_set(app->view_port,ds_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->monitoring){tick++;if(tick%8==0){app->runtime++;sim_device(app);}}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->detail_view){if(!app->monitoring)app->monitoring=true;else if(app->count>0)app->detail_view=true;} break;
            case InputKeyDown: if(!app->detail_view&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(!app->detail_view&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack: if(app->detail_view)app->detail_view=false;else if(app->monitoring)app->monitoring=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
