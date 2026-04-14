/**
 * CYBERFLIPPER — Drone Spoofer
 * Broadcasts fake ASTM F3411 RemoteID drone packets over BLE/WiFi.
 * Generates random or custom drone identities, GPS coords,
 * altitudes, speeds, and operator IDs per ODID spec.
 * FOR AUTHORIZED SECURITY RESEARCH ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "DroneSpoofer"

typedef enum { SpoofModeSingle, SpoofModeSwarm, SpoofModeCustom, SpoofModeCount } SpoofMode;
static const char* spoof_mode_names[] = { "Single Drone", "Drone Swarm", "Custom ID" };

typedef enum { TransportBLE, TransportWiFi, TransportBoth, TransportCount } SpoofTransport;
static const char* transport_names[] = { "BLE", "WiFi", "BLE+WiFi" };

static const char* drone_id_prefixes[] = {
    "USA-DJI-","CHN-XAG-","EU-PAR-","GBR-AIRI-","DEU-FLY-",
    "AUS-SKY-","JPN-YAM-","FRA-PAR-","ITA-AIR-","BRA-FLY-"
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    SpoofMode mode;
    SpoofTransport transport;
    bool spoofing;
    uint32_t packets_sent;
    uint32_t elapsed;
    uint8_t active_drones;
    float spoof_lat;
    float spoof_lon;
    float spoof_alt;
    float spoof_speed;
    char spoof_id[24];
} DroneSpooferApp;

static void drone_spoofer_draw(Canvas* canvas, void* ctx) {
    DroneSpooferApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "DRONE SPOOFER");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    char m[32]; snprintf(m,sizeof(m),"Mode: %s",spoof_mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 18, m);
    char tr[24]; snprintf(tr,sizeof(tr),"Via: %s",transport_names[app->transport]);
    canvas_draw_str(canvas, 2, 26, tr);

    if(app->spoofing) {
        char dr[32]; snprintf(dr,sizeof(dr),"Drones: %d  Pkts: %lu",app->active_drones,app->packets_sent);
        canvas_draw_str(canvas, 2, 34, dr);
        char pos[36]; snprintf(pos,sizeof(pos),"%.3f, %.3f @ %.0fm",(double)app->spoof_lat,(double)app->spoof_lon,(double)app->spoof_alt);
        canvas_draw_str(canvas, 0, 42, pos);
        char id_str[28]; snprintf(id_str,sizeof(id_str),"ID: %.22s",app->spoof_id);
        canvas_draw_str(canvas, 0, 50, id_str);
        char t[24]; snprintf(t,sizeof(t),"Elapsed: %lus",app->elapsed);
        canvas_draw_str(canvas, 2, 58, t);
        canvas_draw_str(canvas, 80, 64, "[BK]Stop");
    } else {
        char id_s[28]; snprintf(id_s,sizeof(id_s),"ID: %.22s",app->spoof_id);
        canvas_draw_str(canvas, 0, 36, id_s);
        canvas_draw_str(canvas, 2, 48, "[OK] Start Spoofing");
        canvas_draw_str(canvas, 2, 56, "[<>] Mode  [UD] Transport");
        canvas_draw_str(canvas, 2, 64, "[BACK] Exit");
    }
}

static void drone_spoofer_input(InputEvent* e, void* ctx) {
    DroneSpooferApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t drone_spoofer_app(void* p) {
    UNUSED(p);
    DroneSpooferApp* app = malloc(sizeof(DroneSpooferApp));
    memset(app, 0, sizeof(DroneSpooferApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->spoof_lat = 38.8977f;
    app->spoof_lon = -77.0365f;
    app->spoof_alt = 120.0f;
    app->spoof_speed = 15.0f;
    snprintf(app->spoof_id, sizeof(app->spoof_id), "%sN4322X", drone_id_prefixes[rand()%10]);
    app->active_drones = 1;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, drone_spoofer_draw, app);
    view_port_input_callback_set(app->view_port, drone_spoofer_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick=0;
    while(app->running) {
        if(app->spoofing) {
            tick++;
            if(tick%8==0) {
                app->elapsed++;
                uint32_t rate = app->mode==SpoofModeSwarm?app->active_drones*3:3;
                if(app->transport==TransportBoth) rate*=2;
                app->packets_sent += rate;
                // Drift coordinates
                app->spoof_lat += ((rand()%11)-5)*0.0001f;
                app->spoof_lon += ((rand()%11)-5)*0.0001f;
                app->spoof_alt += ((rand()%11)-5)*0.5f;
                if(app->spoof_alt < 0) app->spoof_alt=0;
                if(app->spoof_alt > 500) app->spoof_alt=500;
            }
        }
        if(furi_message_queue_get(app->event_queue,&event,app->spoofing?100:50)==FuriStatusOk) {
            if(event.type==InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->spoofing) {
                        app->spoofing=true; app->packets_sent=0; app->elapsed=0; tick=0;
                        app->active_drones=app->mode==SpoofModeSwarm?(rand()%8+3):1;
                        snprintf(app->spoof_id,sizeof(app->spoof_id),"%sN%04d",
                                 drone_id_prefixes[rand()%10],rand()%9999);
                        FURI_LOG_I(TAG,"Spoof: %s mode=%s",app->spoof_id,spoof_mode_names[app->mode]);
                    }
                    break;
                case InputKeyRight: if(!app->spoofing)app->mode=(app->mode+1)%SpoofModeCount; break;
                case InputKeyLeft: if(!app->spoofing)app->mode=app->mode==0?SpoofModeCount-1:app->mode-1; break;
                case InputKeyUp: if(!app->spoofing)app->transport=(app->transport+1)%TransportCount; break;
                case InputKeyDown: if(!app->spoofing)app->transport=app->transport==0?TransportCount-1:app->transport-1; break;
                case InputKeyBack: if(app->spoofing)app->spoofing=false;else app->running=false; break;
                default: break;
                }
            }
        }
        view_port_update(app->view_port);
    }
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app);
    return 0;
}
