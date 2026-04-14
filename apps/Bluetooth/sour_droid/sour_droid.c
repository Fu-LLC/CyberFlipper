/**
 * CYBERFLIPPER — Sour Droid
 * Android/Samsung BLE notification flood. Cycles Google FastPair
 * and Samsung EasySetup advertisements across device models.
 * FOR AUTHORIZED PROTOCOL TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "SourDroid"

typedef enum { SDModeGoogle, SDModeSamsung, SDModeBoth, SDModeCount } SDMode;
static const char* sd_mode_names[]={"Google FastPair","Samsung EasySetup","Both Combined"};

static const char* google_devices[]={"Pixel Buds Pro","Pixel Buds A","JBL Flip 6","Sony WH-1000XM5",
    "Bose QC45","Google Nest Mini","Beats Flex","Nothing Ear 2","OnePlus Buds Pro","Jabra Elite 85t"};
static const char* samsung_devices[]={"Galaxy Buds2 Pro","Galaxy Buds FE","Galaxy Watch6","Galaxy SmartTag2",
    "Galaxy Ring","Galaxy Fit3","Galaxy Buds Live","Galaxy Watch5 Pro","Galaxy Buds+","Galaxy Watch4"};

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    SDMode mode; bool flooding; uint32_t packets; uint32_t elapsed;
    uint16_t model_idx; uint32_t models_cycled;
} SourDroidApp;

static void sd_draw(Canvas* canvas, void* ctx) {
    SourDroidApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"SOUR DROID");
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    char m[32]; snprintf(m,sizeof(m),"Mode: %s",sd_mode_names[app->mode]); canvas_draw_str(canvas,2,18,m);
    if(app->flooding) {
        const char* cur_dev=app->mode==SDModeSamsung?samsung_devices[app->model_idx%10]:google_devices[app->model_idx%10];
        char d[32]; snprintf(d,sizeof(d),"Device: %s",cur_dev); canvas_draw_str(canvas,2,28,d);
        char p[32]; snprintf(p,sizeof(p),"Packets: %lu",app->packets); canvas_draw_str(canvas,2,38,p);
        char mc[32]; snprintf(mc,sizeof(mc),"Models cycled: %lu",app->models_cycled); canvas_draw_str(canvas,2,46,mc);
        canvas_draw_str_aligned(canvas,64,54,AlignCenter,AlignTop,">> FLOODING <<");
        canvas_draw_str(canvas,30,64,"[BACK] Stop");
    } else {
        canvas_draw_str(canvas,2,30,"Floods Android/Samsung");
        canvas_draw_str(canvas,2,38,"with fake pair requests");
        canvas_draw_str(canvas,2,48,"[OK]Start [<>]Mode");
        canvas_draw_str(canvas,2,58,"[BACK]Exit");
    }
}
static void sd_input(InputEvent* e,void* ctx){SourDroidApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t sour_droid_app(void* p) {
    UNUSED(p);
    SourDroidApp* app=malloc(sizeof(SourDroidApp)); memset(app,0,sizeof(SourDroidApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,sd_draw,app);
    view_port_input_callback_set(app->view_port,sd_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->flooding){tick++;if(tick%3==0){app->packets+=3;app->model_idx++;if(app->model_idx%10==0)app->models_cycled++;}if(tick%10==0)app->elapsed++;}
        if(furi_message_queue_get(app->event_queue,&event,app->flooding?80:50)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(!app->flooding){app->flooding=true;app->packets=0;app->elapsed=0;app->model_idx=0;app->models_cycled=0;tick=0;} break;
            case InputKeyRight: if(!app->flooding)app->mode=(app->mode+1)%SDModeCount; break;
            case InputKeyLeft: if(!app->flooding)app->mode=app->mode==0?SDModeCount-1:app->mode-1; break;
            case InputKeyBack: if(app->flooding)app->flooding=false;else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
