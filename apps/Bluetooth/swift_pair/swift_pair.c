/**
 * CYBERFLIPPER — Swift Pair
 * Triggers Windows Swift Pair pairing notifications by
 * broadcasting fake Microsoft BLE device advertisements.
 * FOR AUTHORIZED TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "SwiftPair"

static const char* ms_devices[]={"Xbox Controller","Surface Earbuds","Surface Headphones 2",
    "Arc Mouse","Sculpt Mouse","Precision Mouse","Designer Keyboard","Number Pad",
    "Modern Webcam","Modern Speaker"};

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    bool broadcasting; uint32_t packets; uint32_t elapsed; uint8_t device_idx; bool auto_cycle;
} SwiftPairApp;

static void sp_draw(Canvas* canvas, void* ctx) {
    SwiftPairApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"SWIFT PAIR");
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    char d[32]; snprintf(d,sizeof(d),"Device: %s",ms_devices[app->device_idx%10]); canvas_draw_str(canvas,2,20,d);
    canvas_draw_str(canvas,2,28,app->auto_cycle?"Mode: Auto-Cycle":"Mode: Fixed Device");
    if(app->broadcasting) {
        char p[24]; snprintf(p,sizeof(p),"Packets: %lu",app->packets); canvas_draw_str(canvas,2,40,p);
        canvas_draw_str_aligned(canvas,64,48,AlignCenter,AlignTop,">> Windows targets! <<");
        canvas_draw_str(canvas,30,62,"[BACK] Stop");
    } else {
        canvas_draw_str(canvas,2,42,"Targets: Windows 10/11");
        canvas_draw_str(canvas,2,50,"[OK]Start [<>]Device");
        canvas_draw_str(canvas,2,58,"[UD]Mode [BK]Exit");
    }
}
static void sp_input(InputEvent* e,void* ctx){SwiftPairApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t swift_pair_app(void* p) {
    UNUSED(p);
    SwiftPairApp* app=malloc(sizeof(SwiftPairApp)); memset(app,0,sizeof(SwiftPairApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,sp_draw,app);
    view_port_input_callback_set(app->view_port,sp_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->broadcasting){tick++;if(tick%5==0){app->packets+=2;app->elapsed++;if(app->auto_cycle&&tick%15==0)app->device_idx++;}}
        if(furi_message_queue_get(app->event_queue,&event,app->broadcasting?100:50)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->broadcasting){app->broadcasting=true;app->packets=0;app->elapsed=0;tick=0;} break;
            case InputKeyRight: if(!app->broadcasting)app->device_idx=(app->device_idx+1)%10; break;
            case InputKeyLeft: if(!app->broadcasting)app->device_idx=app->device_idx==0?9:app->device_idx-1; break;
            case InputKeyUp: app->auto_cycle=!app->auto_cycle; break;
            case InputKeyBack: if(app->broadcasting)app->broadcasting=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
