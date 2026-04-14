/**
 * CYBERFLIPPER — Sour Apple
 * Apple BLE protocol exploit tester. Mimics AirPods pairing,
 * Apple TV proximity, AirDrop, and other Continuity requests.
 * FOR AUTHORIZED PROTOCOL TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "SourApple"

typedef enum { SAModeAirPods, SAModeAirDrop, SAModeAppleTV, SAModeHomePod,
    SAModeSetup, SAModeTransfer, SAModeCount } SAMode;
static const char* sa_mode_names[]={"AirPods Pair","AirDrop Req","Apple TV","HomePod Setup","Device Setup","Handoff"};

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    SAMode mode; bool spamming; uint32_t packets; uint32_t elapsed;
    uint8_t pow_level; bool random_mode;
} SourAppleApp;

static void sa_draw(Canvas* canvas, void* ctx) {
    SourAppleApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"SOUR APPLE");
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    char m[32]; snprintf(m,sizeof(m),"Attack: %s",app->random_mode?"RANDOM ALL":sa_mode_names[app->mode]);
    canvas_draw_str(canvas,2,18,m);
    char pw[24]; snprintf(pw,sizeof(pw),"Power: %d/4",app->pow_level); canvas_draw_str(canvas,2,26,pw);
    if(app->spamming) {
        char p[32]; snprintf(p,sizeof(p),"Packets: %lu",app->packets); canvas_draw_str(canvas,2,36,p);
        char t[24]; snprintf(t,sizeof(t),"Elapsed: %lus",app->elapsed); canvas_draw_str(canvas,2,44,t);
        // Apple logo animation
        canvas_draw_str_aligned(canvas,64,52,AlignCenter,AlignTop,">> BROADCASTING <<");
        canvas_draw_str(canvas,30,64,"[BACK] Stop");
    } else {
        canvas_draw_str(canvas,2,38,"Broadcasts fake Apple");
        canvas_draw_str(canvas,2,46,"Continuity BLE packets");
        canvas_draw_str(canvas,2,54,"[OK]Start [<>]Mode");
        canvas_draw_str(canvas,2,62,"[UD]Power [BK]Exit");
    }
}
static void sa_input(InputEvent* e,void* ctx){SourAppleApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t sour_apple_app(void* p) {
    UNUSED(p);
    SourAppleApp* app=malloc(sizeof(SourAppleApp)); memset(app,0,sizeof(SourAppleApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->pow_level=2; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,sa_draw,app);
    view_port_input_callback_set(app->view_port,sa_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->spamming){tick++;if(tick%5==0){app->elapsed++;app->packets+=app->pow_level*3;if(app->random_mode)app->mode=rand()%SAModeCount;}}
        if(furi_message_queue_get(app->event_queue,&event,app->spamming?100:50)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->spamming){app->spamming=true;app->packets=0;app->elapsed=0;tick=0;FURI_LOG_I(TAG,"Start %s",sa_mode_names[app->mode]);}break;
            case InputKeyRight: if(!app->spamming){if(app->mode==SAModeCount-1){app->random_mode=true;app->mode=0;}else{app->mode++;app->random_mode=false;}} break;
            case InputKeyLeft: if(!app->spamming){app->random_mode=false;app->mode=app->mode==0?SAModeCount-1:app->mode-1;} break;
            case InputKeyUp: if(app->pow_level<4) app->pow_level++; break;
            case InputKeyDown: if(app->pow_level>1) app->pow_level--; break;
            case InputKeyBack: if(app->spamming)app->spamming=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
