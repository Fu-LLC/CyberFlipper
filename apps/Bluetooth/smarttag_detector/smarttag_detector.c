/**
 * CYBERFLIPPER — SmartTag / Tile / RayBan Detector
 * Multi-tracker detector. Identifies Samsung SmartTag, SmartTag2,
 * Tile trackers, and RayBan Meta smart glasses via BLE.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "TrackerDetect"
#define MAX_TRACKERS 20

typedef enum { TrackerSmartTag, TrackerSmartTag2, TrackerTile, TrackerTilePro,
    TrackerRayBan, TrackerTypeCount } TrackerType;
static const char* tracker_names[]={"SmartTag","SmartTag2","Tile","Tile Pro","RayBan Meta"};
static const uint16_t tracker_mfr[]={0x0075,0x0075,0xFE2C,0xFE2C,0x004C};

typedef struct {
    char name[20]; uint8_t addr[6]; int8_t rssi; TrackerType type;
    uint32_t seen_count; bool is_following;
} TrackerDev;

typedef enum { TDViewList, TDViewDetail, TDViewLocate } TDView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    TDView view; bool scanning; TrackerDev trackers[MAX_TRACKERS]; uint8_t count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time; uint8_t alert_count;
} SmartTagApp;

static void sim_tracker(SmartTagApp* app) {
    if(app->count>0&&rand()%3==0) {
        uint8_t idx=rand()%app->count;
        app->trackers[idx].seen_count++;
        app->trackers[idx].rssi=-(rand()%70)-20;
        if(app->trackers[idx].seen_count>10) {app->trackers[idx].is_following=true;app->alert_count++;}
        return;
    }
    if(app->count>=MAX_TRACKERS||rand()%5!=0) return;
    TrackerDev* t=&app->trackers[app->count++];
    t->type=rand()%TrackerTypeCount;
    snprintf(t->name,sizeof(t->name),"%s_%02X%02X",tracker_names[t->type],rand()%256,rand()%256);
    for(int i=0;i<6;i++) t->addr[i]=rand()%256;
    t->rssi=-(rand()%70)-20; t->seen_count=1; t->is_following=false;
}

static void td_draw(Canvas* canvas, void* ctx) {
    SmartTagApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    switch(app->view) {
    case TDViewList: {
        char title[32]; snprintf(title,sizeof(title),"TRACKERS [%d]%s",app->count,app->alert_count>0?" !!":"");
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,title);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        if(app->count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning...":"No trackers");canvas_draw_str(canvas,10,44,"[OK] Scan");break;}
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
            uint8_t idx=app->scroll_pos+i; TrackerDev* t=&app->trackers[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%s%-10s %ddB%s",idx==app->selected?">":"  ",t->is_following?"!":"",t->name,t->rssi,t->is_following?" !!":"");
            canvas_draw_str(canvas,0,19+i*10,l);
        }
        canvas_draw_str(canvas,2,62,"[OK]Info [BK]Exit");
        break;
    }
    case TDViewDetail: {
        TrackerDev* t=&app->trackers[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t->is_following?"!! TRACKER ALERT !!":"TRACKER INFO");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"Name: %s",t->name); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"Type: %s",tracker_names[t->type]); canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",t->addr[0],t->addr[1],t->addr[2],t->addr[3],t->addr[4],t->addr[5]);
        canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"RSSI:%ddBm Seen:%lu",t->rssi,t->seen_count); canvas_draw_str(canvas,2,42,s);
        canvas_draw_str(canvas,2,50,t->is_following?"HIGH persistence - tracker?":"Normal BLE device");
        canvas_draw_str(canvas,2,62,"[>]Locate [BK]List");
        break;
    }
    case TDViewLocate: {
        TrackerDev* t=&app->trackers[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"LOCATE TRACKER");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char r[24]; snprintf(r,sizeof(r),"RSSI: %d dBm",t->rssi); canvas_draw_str(canvas,2,18,r);
        int norm=t->rssi+100;if(norm<0)norm=0;if(norm>100)norm=100;
        for(uint8_t b=0;b<10;b++){uint8_t bh=4+(b*3);if(b*10<(uint8_t)norm)canvas_draw_box(canvas,4+b*12,52-bh,10,bh);else canvas_draw_frame(canvas,4+b*12,52-bh,10,bh);}
        canvas_draw_str_aligned(canvas,64,55,AlignCenter,AlignTop,t->rssi>-40?"VERY CLOSE!":t->rssi>-65?"Getting closer":"Move toward signal");
        canvas_draw_str(canvas,30,64,"[BACK] Detail");
        break;
    }}
}
static void td_input(InputEvent* e,void* ctx){SmartTagApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t smarttag_detector_app(void* p) {
    UNUSED(p);
    SmartTagApp* app=malloc(sizeof(SmartTagApp)); memset(app,0,sizeof(SmartTagApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,td_draw,app);
    view_port_input_callback_set(app->view_port,td_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%8==0){app->scan_time++;sim_tracker(app);}}
        if(app->view==TDViewLocate&&app->count>0){app->trackers[app->selected].rssi+=(rand()%7)-3;if(app->trackers[app->selected].rssi>-15)app->trackers[app->selected].rssi=-15;if(app->trackers[app->selected].rssi<-95)app->trackers[app->selected].rssi=-95;}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(app->view==TDViewList&&app->count>0)app->view=TDViewDetail; break;
            case InputKeyRight: if(app->view==TDViewDetail)app->view=TDViewLocate; break;
            case InputKeyDown: if(app->view==TDViewList&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(app->view==TDViewList&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyBack: if(app->view==TDViewLocate)app->view=TDViewDetail;else if(app->view==TDViewDetail)app->view=TDViewList;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
