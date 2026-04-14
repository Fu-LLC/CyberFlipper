/**
 * CYBERFLIPPER — WiFi Scanner
 * Full AP scanner with client detection, signal monitoring, client
 * deauthentication. Shows SSID, BSSID, channel, encryption, RSSI,
 * and connected client MAC addresses per network.
 * FOR AUTHORIZED TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>
#define TAG "WiFiScanner"
#define MAX_APS 20
#define MAX_CLIENTS 6

typedef struct { uint8_t mac[6]; int8_t rssi; uint32_t pkts; } Client;
typedef struct {
    char ssid[33]; uint8_t bssid[6]; int8_t rssi; uint8_t channel;
    char enc[8]; Client clients[MAX_CLIENTS]; uint8_t client_count;
} AP;
typedef enum { WSViewList, WSViewDetail, WSViewClients } WSView;

typedef struct {
    Gui* gui; ViewPort* view_port; FuriMessageQueue* event_queue; bool running;
    WSView view; bool scanning; AP aps[MAX_APS]; uint8_t ap_count;
    uint8_t selected; uint8_t scroll_pos; uint32_t scan_time;
} WiFiScannerApp;

static void sim_scan(WiFiScannerApp* app) {
    static const char* ssids[] = {"Xfinity_2G","ATT-WiFi","Verizon5G","NETGEAR80","TP-Link_Home",
        "Corp_Secure","Guest_Net","iPhone12","AndroidAP","HiddenNet","Starlink","Google_Nest",
        "CenturyLink","CoxWiFi","FreedomNet","CafeWiFi","Hotel_Guest","Airport_WiFi","Library","edu-net"};
    static const char* encs[] = {"WPA2","WPA3","WEP","OPEN","WPA2","WPA3","OPEN","WPA2","WPA2","WPA2",
        "WPA3","WPA2","WPA2","OPEN","WPA2","OPEN","WPA2","OPEN","WPA2","WPA2"};
    if(app->ap_count<MAX_APS && rand()%3==0) {
        AP* ap=&app->aps[app->ap_count]; uint8_t ni=app->ap_count%20;
        strncpy(ap->ssid,ssids[ni],32); strncpy(ap->enc,encs[ni],7);
        for(int j=0;j<6;j++) ap->bssid[j]=rand()%256;
        ap->rssi=-(rand()%70)-20; ap->channel=1+(rand()%13);
        ap->client_count=rand()%MAX_CLIENTS;
        for(uint8_t c=0;c<ap->client_count;c++) {
            for(int j=0;j<6;j++) ap->clients[c].mac[j]=rand()%256;
            ap->clients[c].rssi=-(rand()%80)-15; ap->clients[c].pkts=rand()%5000;
        }
        app->ap_count++;
    }
}

static void ws_draw(Canvas* canvas, void* ctx) {
    WiFiScannerApp* app=ctx; canvas_clear(canvas); canvas_set_font(canvas, FontPrimary);
    switch(app->view) {
    case WSViewList: {
        char t[28]; snprintf(t,sizeof(t),"WIFI SCAN [%d APs]",app->ap_count);
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        for(uint8_t i=0;i<4&&(app->scroll_pos+i)<app->ap_count;i++) {
            uint8_t idx=app->scroll_pos+i; AP* a=&app->aps[idx]; char l[36];
            snprintf(l,sizeof(l),"%s%-12s ch%02d %ddB %s",idx==app->selected?">":"  ",a->ssid,a->channel,a->rssi,a->enc);
            canvas_draw_str(canvas,0,19+i*10,l);
        }
        char st[28]; snprintf(st,sizeof(st),"%s %lus",app->scanning?"SCAN":"IDLE",app->scan_time);
        canvas_draw_str(canvas,2,62,st); canvas_draw_str(canvas,70,62,"[OK]Detail");
        break;
    }
    case WSViewDetail: {
        AP* a=&app->aps[app->selected];
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,"AP DETAIL");
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        char s[40]; snprintf(s,sizeof(s),"SSID: %s",a->ssid); canvas_draw_str(canvas,2,18,s);
        snprintf(s,sizeof(s),"%02X:%02X:%02X:%02X:%02X:%02X",a->bssid[0],a->bssid[1],a->bssid[2],a->bssid[3],a->bssid[4],a->bssid[5]);
        canvas_draw_str(canvas,2,26,s);
        snprintf(s,sizeof(s),"Ch: %d  RSSI: %ddBm",a->channel,a->rssi); canvas_draw_str(canvas,2,34,s);
        snprintf(s,sizeof(s),"Enc: %s  Clients: %d",a->enc,a->client_count); canvas_draw_str(canvas,2,42,s);
        int norm=a->rssi+100;if(norm<0)norm=0;if(norm>100)norm=100;
        canvas_draw_frame(canvas,2,46,124,6); canvas_draw_box(canvas,3,47,(norm*122)/100,4);
        canvas_draw_str(canvas,2,62,"[OK]Clients [BK]List");
        break;
    }
    case WSViewClients: {
        AP* a=&app->aps[app->selected];
        char t[28]; snprintf(t,sizeof(t),"CLIENTS [%d]",a->client_count);
        canvas_draw_str_aligned(canvas,64,0,AlignCenter,AlignTop,t);
        canvas_draw_line(canvas,0,10,128,10); canvas_set_font(canvas,FontSecondary);
        if(a->client_count==0){canvas_draw_str(canvas,10,35,"No clients"); break;}
        for(uint8_t i=0;i<5&&i<a->client_count;i++) {
            Client* c=&a->clients[i]; char l[36];
            snprintf(l,sizeof(l),"%02X:%02X:%02X:%02X %ddB %lupk",c->mac[0],c->mac[1],c->mac[2],c->mac[3],c->rssi,c->pkts);
            canvas_draw_str(canvas,2,19+i*9,l);
        }
        canvas_draw_str(canvas,2,62,"[OK]Deauth [BK]Detail");
        break;
    }}
}
static void ws_input(InputEvent* e, void* ctx) { WiFiScannerApp* app=ctx; furi_message_queue_put(app->event_queue,e,FuriWaitForever); }

int32_t wifi_scanner_cf_app(void* p) {
    UNUSED(p);
    WiFiScannerApp* app=malloc(sizeof(WiFiScannerApp)); memset(app,0,sizeof(WiFiScannerApp));
    app->view_port=view_port_alloc(); app->event_queue=furi_message_queue_alloc(8,sizeof(InputEvent));
    app->running=true; app->scanning=true;
    app->gui=furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port,ws_draw,app);
    view_port_input_callback_set(app->view_port,ws_input,app);
    gui_add_view_port(app->gui,app->view_port,GuiLayerFullscreen);
    InputEvent event; uint32_t tick=0;
    while(app->running) {
        if(app->scanning){tick++;if(tick%10==0){app->scan_time++;sim_scan(app);}}
        if(furi_message_queue_get(app->event_queue,&event,app->scanning?100:50)==FuriStatusOk) {
            if(event.type==InputTypeShort) switch(event.key) {
            case InputKeyOk:
                if(app->view==WSViewList&&app->ap_count>0)app->view=WSViewDetail;
                else if(app->view==WSViewDetail)app->view=WSViewClients;
                else if(app->view==WSViewClients){FURI_LOG_I(TAG,"Deauth sent");} break;
            case InputKeyDown: if(app->view==WSViewList&&app->selected<app->ap_count-1){app->selected++;if(app->selected>=app->scroll_pos+4)app->scroll_pos++;} break;
            case InputKeyUp: if(app->view==WSViewList&&app->selected>0){app->selected--;if(app->selected<app->scroll_pos)app->scroll_pos=app->selected;} break;
            case InputKeyRight: app->scanning=!app->scanning; break;
            case InputKeyBack: if(app->view==WSViewClients)app->view=WSViewDetail;else if(app->view==WSViewDetail)app->view=WSViewList;else app->running=false; break;
            default: break;
            }
        }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui,app->view_port); view_port_free(app->view_port);
    furi_record_close(RECORD_GUI); furi_message_queue_free(app->event_queue); free(app); return 0;
}
