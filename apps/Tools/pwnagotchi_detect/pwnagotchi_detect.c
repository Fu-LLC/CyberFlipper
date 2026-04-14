/**
 * CYBERFLIPPER — Pwnagotchi Detector + Spam
 * Detects Pwnagotchi AI WiFi audit devices.
 * Spam mode floods the Pwnagotchi mesh with fake identities.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "PwnDetect"
#define MAX_PWNS 8

static const char* pwn_faces[]={"(^_^)","(>_<)","(;_;)","(o_O)","(*_*)","(T_T)","(=_=)",":D"};
static const char* pwn_names[]={"pwnzero","hackbot","wifi_nomad","packet_muncher","deauth_king",
    "airsnort","beacon_eater","channel_hopper","probe_hunter","wpa_cracker"};

typedef struct { char name[20]; char face[8]; uint8_t version; int8_t rssi; uint8_t channel; uint32_t uptime; uint16_t deauths; } PwnEntry;
typedef enum { PwnViewDetect, PwnViewSpam } PwnView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    PwnView view; bool scanning; bool spamming;
    PwnEntry pwns[MAX_PWNS]; uint8_t pwn_count; uint8_t selected; uint8_t scroll_pos;
    uint32_t scan_time; uint32_t spam_count; bool dos_mode;
} PwnagotchiApp;

static void sim_pwn(PwnagotchiApp* app) {
    if(app->pwn_count>=MAX_PWNS||rand()%8!=0) return;
    PwnEntry* p=&app->pwns[app->pwn_count++];
    strncpy(p->name,pwn_names[rand()%10],19); strncpy(p->face,pwn_faces[rand()%8],7);
    p->version=14+rand()%5; p->rssi=-(rand()%60)-25; p->channel=1+rand()%13;
    p->uptime=(rand()%3600)*60; p->deauths=rand()%5000;
}

static void pwn_draw(Canvas* canvas, void* ctx) {
    PwnagotchiApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    if(app->view==PwnViewSpam) {
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"PWNAGOTCHI SPAM");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[32]; snprintf(s,sizeof(s),"Fake beacons: %lu",app->spam_count); canvas_draw_str(canvas,2,22,s);
        canvas_draw_str(canvas,2,32,app->dos_mode?"Mode: DoS FLOOD":"Mode: Grid Spam");
        canvas_draw_str(canvas,2,42,app->spamming?">> SPAMMING <<":"[OK] Start");
        if(app->spamming) {
            char anim[20]; for(int i=0;i<16;i++) anim[i]=pwn_faces[rand()%8][rand()%3]; anim[16]=0;
            canvas_draw_str(canvas,2,52,anim);
        }
        canvas_draw_str(canvas,2,62,"[UD]Mode [BK]Detect");
        return;
    }
    char t[28]; snprintf(t,sizeof(t),"PWNAGOTCHI [%d]",app->pwn_count);
    canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
    canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
    if(app->pwn_count==0){canvas_draw_str(canvas,10,32,app->scanning?"Scanning...":"No Pwnagotchis");canvas_draw_str(canvas,10,44,"[OK]Scan [>]Spam"); break_draw:;
    } else {
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->pwn_count;i++){
            uint8_t idx=app->scroll_pos+i; PwnEntry* p=&app->pwns[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%s %-10s v%d %ddB",idx==app->selected?">":"  ",p->face,p->name,p->version,p->rssi);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
    }
    char st[24]; snprintf(st,sizeof(st),"%lus",app->scan_time); canvas_draw_str(canvas,2,62,st);
    canvas_draw_str(canvas,50,62,"[>]Spam [OK]Scan");
}
static void pwn_input(InputEvent* e,void* ctx){PwnagotchiApp* app=ctx;furi_message_queue_put(app->event_queue,e,FuriWaitForever);}

int32_t pwnagotchi_detect_app(void* p) {
    UNUSED(p);
    PwnagotchiApp* app=malloc(sizeof(PwnagotchiApp)); memset(app,0,sizeof(PwnagotchiApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,pwn_draw,app);
    view_port_input_callback_set(app->view_port,pwn_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%12==0){app->scan_time++;sim_pwn(app);}}
        if(app->spamming){app->spam_count+=app->dos_mode?50:5;}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk: if(app->view==PwnViewDetect){app->scanning=!app->scanning;tick=0;app->scan_time=0;}else app->spamming=!app->spamming; break;
            case InputKeyRight: if(app->view==PwnViewDetect)app->view=PwnViewSpam; break;
            case InputKeyUp: if(app->view==PwnViewSpam)app->dos_mode=!app->dos_mode;else if(app->selected>0)app->selected--; break;
            case InputKeyDown: if(app->view==PwnViewDetect&&app->selected<app->pwn_count-1)app->selected++; break;
            case InputKeyBack: if(app->view==PwnViewSpam){app->view=PwnViewDetect;app->spamming=false;}else app->running=false; break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
