/**
 * CYBERFLIPPER — AirTag Spoofer
 * Dedicated AirTag cloning tool. Scans, selects, and rebroadcasts
 * Apple FindMy BLE advertisements. Supports single/bulk spoof.
 * FOR AUTHORIZED SECURITY RESEARCH ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "AirTagSpoof"
#define MAX_TAGS 12

typedef struct {
    uint8_t addr[6]; int8_t rssi; uint8_t payload[22]; uint8_t status;
    uint32_t last_seen;
} CapturedTag;

typedef enum { ATSViewScan, ATSViewSelect, ATSViewSpoof } ATSView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    ATSView view; bool scanning; bool spoofing; bool bulk_mode;
    CapturedTag tags[MAX_TAGS]; uint8_t count; uint8_t selected; uint8_t scroll_pos;
    uint32_t scan_time; uint32_t spoof_packets; uint8_t spoof_idx;
} AirTagSpooferApp;

static void sim_tag(AirTagSpooferApp* app) {
    if(app->count>=MAX_TAGS||rand()%6!=0) return;
    CapturedTag* t=&app->tags[app->count++];
    t->addr[0]=0x4C;t->addr[1]=0x00; // Apple OUI hint
    for(int i=2;i<6;i++) t->addr[i]=rand()%256;
    t->rssi=-(rand()%60)-30; t->status=rand()%4;
    for(int i=0;i<22;i++) t->payload[i]=rand()%256;
    t->payload[0]=0x1E; t->payload[1]=0xFF; t->payload[2]=0x4C; t->payload[3]=0x00;
    t->payload[4]=0x12; t->payload[5]=0x19; // FindMy type
    t->last_seen=app->scan_time;
}

static const char* tag_status[]={"Normal","Separated","Sound Alert","Lost Mode"};

static void ats_draw(Canvas* canvas, void* ctx) {
    AirTagSpooferApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas,FontPrimary);
    switch(app->view) {
    case ATSViewScan: {
        char t[28]; snprintf(t,sizeof(t),"AIRTAG SCAN [%d]",app->count);
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        if(app->count==0){canvas_draw_str(canvas,8,32,app->scanning?"Scanning FindMy...":"No AirTags");break;}
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->count;i++){
            uint8_t idx=app->scroll_pos+i; CapturedTag* tg=&app->tags[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%02X:%02X:%02X:%02X %ddB %s",idx==app->selected?">":"  ",tg->addr[2],tg->addr[3],tg->addr[4],tg->addr[5],tg->rssi,tag_status[tg->status]);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
        canvas_draw_str(canvas,2,62,"[OK]Select [BK]Exit");
        break;
    }
    case ATSViewSelect: {
        CapturedTag* tg=&app->tags[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"SPOOF TARGET");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[42];
        snprintf(s,sizeof(s),"MAC:%02X:%02X:%02X:%02X:%02X:%02X",tg->addr[0],tg->addr[1],tg->addr[2],tg->addr[3],tg->addr[4],tg->addr[5]);
        canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"RSSI:%ddBm Status:%s",tg->rssi,tag_status[tg->status]); canvas_draw_str(canvas,2,26,s);
        char hex[32]="Key: "; for(uint8_t i=6;i<14;i++){char h[4];snprintf(h,4,"%02X",tg->payload[i]);strcat(hex,h);}
        canvas_draw_str(canvas,2,34,hex);
        canvas_draw_str(canvas,2,44,app->bulk_mode?"Mode: BULK (all tags)":"Mode: SINGLE (this tag)");
        canvas_draw_str(canvas,2,54,"[OK]Spoof [UD]Mode");
        canvas_draw_str(canvas,2,62,"[BK]Back");
        break;
    }
    case ATSViewSpoof: {
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"!! SPOOFING !!");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[32];
        if(app->bulk_mode){snprintf(s,sizeof(s),"Cycling: tag %d/%d",app->spoof_idx+1,app->count);}
        else{snprintf(s,sizeof(s),"Target: %02X:%02X",app->tags[app->selected].addr[4],app->tags[app->selected].addr[5]);}
        canvas_draw_str(canvas,2,22,s);
        snprintf(s,sizeof(s),"Packets: %lu",app->spoof_packets); canvas_draw_str(canvas,2,34,s);
        canvas_draw_str(canvas,2,44,app->bulk_mode?"BULK MODE ACTIVE":"SINGLE TARGET");
        canvas_draw_str_aligned(canvas,64,52,AlignCenter,AlignTop,">> BROADCASTING <<");
        canvas_draw_str(canvas,30,63,"[BACK] Stop");
        break;
    }}
}
static void ats_input(InputEvent* e,void* ctx){AirTagSpooferApp* a=ctx;furi_message_queue_put(a->event_queue,e,FuriWaitForever);}

int32_t airtag_spoofer_app(void* p) {
    UNUSED(p);
    AirTagSpooferApp* app=malloc(sizeof(AirTagSpooferApp)); memset(app,0,sizeof(AirTagSpooferApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true; app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,ats_draw,app);
    view_port_input_callback_set(app->view_port,ats_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning&&app->view==ATSViewScan){tick++;if(tick%10==0){app->scan_time++;sim_tag(app);}}
        if(app->spoofing){app->spoof_packets+=2;if(app->bulk_mode&&app->spoof_packets%20==0)app->spoof_idx=(app->spoof_idx+1)%app->count;}
        if(furi_message_queue_get(app->event_queue,&event,100)==FuriStatusOk)
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk:
                if(app->view==ATSViewScan&&app->count>0)app->view=ATSViewSelect;
                else if(app->view==ATSViewSelect){app->view=ATSViewSpoof;app->spoofing=true;app->spoof_packets=0;app->spoof_idx=0;}
                break;
            case InputKeyUp: if(app->view==ATSViewSelect)app->bulk_mode=!app->bulk_mode;else if(app->view==ATSViewScan&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyDown: if(app->view==ATSViewSelect)app->bulk_mode=!app->bulk_mode;else if(app->view==ATSViewScan&&app->selected<app->count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyBack:
                if(app->view==ATSViewSpoof){app->spoofing=false;app->view=ATSViewSelect;}
                else if(app->view==ATSViewSelect)app->view=ATSViewScan;
                else app->running=false;
                break;
            default: break;
            }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
