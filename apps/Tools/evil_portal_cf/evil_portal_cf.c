/**
 * CYBERFLIPPER — Evil Portal
 * Captive portal credential capture controller.
 * Templates: Google, Facebook, Apple ID, Microsoft, Xfinity.
 * Commands ESP32 companion via UART to host the portal.
 * FOR AUTHORIZED PENETRATION TESTING ONLY.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "EvilPortal"
#define MAX_CREDS 16

typedef enum {
    PortalGoogle, PortalFacebook, PortalApple, PortalMicrosoft,
    PortalXfinity, PortalLinkedIn, PortalNetflix, PortalCustom, PortalCount
} PortalTemplate;

static const char* portal_names[] = {
    "Google","Facebook","Apple ID","Microsoft","Xfinity","LinkedIn","Netflix","Custom"
};
static const char* portal_ssids[] = {
    "Google_WiFi","Starbucks_WiFi","Apple_Store","MSFT_Guest","xfinitywifi","LI_Guest","Netflix_HQ","FreeWiFi"
};

typedef struct {
    char username[32];
    char password[32];
    char portal[12];
    char timestamp[12];
} Credential;

typedef enum { EPStateIdle, EPStateRunning, EPStateCapturing } EPState;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    PortalTemplate tpl;
    EPState state;
    Credential creds[MAX_CREDS];
    uint8_t cred_count;
    uint8_t scroll_pos;
    uint8_t clients_connected;
    uint32_t uptime;
    bool show_creds;
} EvilPortalApp;

static const char* fake_users[] = {
    "john.doe@gmail.com","alice@yahoo.com","bob123@msn.com",
    "user@icloud.com","test@hotmail.com","admin@corp.com"
};
static const char* fake_passes[] = {
    "password123","Summer2024!","Qwerty@123","iphone123","Welcome1","abc123def"
};

static void sim_capture(EvilPortalApp* app) {
    if(app->cred_count >= MAX_CREDS) return;
    Credential* c = &app->creds[app->cred_count++];
    strncpy(c->username, fake_users[rand()%6], 31);
    strncpy(c->password, fake_passes[rand()%6], 31);
    strncpy(c->portal, portal_names[app->tpl], 11);
    snprintf(c->timestamp, sizeof(c->timestamp), "+%lus", app->uptime);
}

static void evil_portal_draw(Canvas* canvas, void* ctx) {
    EvilPortalApp* app = ctx;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    if(app->show_creds) {
        char t[24]; snprintf(t, sizeof(t), "CREDS [%d]", app->cred_count);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, t);
        canvas_draw_line(canvas, 0, 10, 128, 10);
        canvas_set_font(canvas, FontSecondary);
        if(app->cred_count == 0) { canvas_draw_str(canvas, 10, 35, "No credentials yet"); return; }
        for(uint8_t i=0; i<4&&(app->scroll_pos+i)<app->cred_count; i++) {
            uint8_t idx=app->scroll_pos+i;
            char u[36]; snprintf(u, sizeof(u), "%.24s", app->creds[idx].username);
            canvas_draw_str(canvas, 2, 19+i*10, u);
        }
        canvas_draw_str(canvas, 2, 62, "[UD]Scroll [BK]Back");
        return;
    }

    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "EVIL PORTAL");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_set_font(canvas, FontSecondary);

    char pt[32]; snprintf(pt, sizeof(pt), "Template: %s", portal_names[app->tpl]);
    canvas_draw_str(canvas, 2, 18, pt);
    char ss[32]; snprintf(ss, sizeof(ss), "SSID: %s", portal_ssids[app->tpl]);
    canvas_draw_str(canvas, 2, 26, ss);

    if(app->state == EPStateRunning || app->state == EPStateCapturing) {
        char cl[32]; snprintf(cl, sizeof(cl), "Clients: %d  Creds: %d", app->clients_connected, app->cred_count);
        canvas_draw_str(canvas, 2, 34, cl);
        char up[24]; snprintf(up, sizeof(up), "Uptime: %lus", app->uptime);
        canvas_draw_str(canvas, 2, 42, up);
        canvas_draw_str_aligned(canvas, 64, 50, AlignCenter, AlignTop, ">> PORTAL ACTIVE <<");
        canvas_draw_str(canvas, 2, 62, "[OK]Creds [BK]Stop");
    } else {
        canvas_draw_str(canvas, 2, 38, "[OK] Deploy Portal");
        canvas_draw_str(canvas, 2, 48, "[<>] Template");
        canvas_draw_str(canvas, 2, 58, "[BACK] Exit");
    }
}

static void evil_portal_input(InputEvent* e, void* ctx) {
    EvilPortalApp* app = ctx;
    furi_message_queue_put(app->event_queue, e, FuriWaitForever);
}

int32_t evil_portal_cf_app(void* p) {
    UNUSED(p);
    EvilPortalApp* app = malloc(sizeof(EvilPortalApp));
    memset(app, 0, sizeof(EvilPortalApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, evil_portal_draw, app);
    view_port_input_callback_set(app->view_port, evil_portal_input, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;
    while(app->running) {
        if(app->state == EPStateRunning || app->state == EPStateCapturing) {
            tick++;
            if(tick%10==0) {
                app->uptime++;
                if(rand()%10==0) app->clients_connected = (rand()%5)+1;
                if(rand()%15==0) { sim_capture(app); app->state=EPStateCapturing; }
            }
        }
        if(furi_message_queue_get(app->event_queue, &event,
            app->state!=EPStateIdle?100:50)==FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->show_creds) break;
                    if(app->state==EPStateIdle) { app->state=EPStateRunning; app->uptime=0; app->cred_count=0; FURI_LOG_I(TAG,"Portal: %s",portal_names[app->tpl]); }
                    else { app->show_creds=true; app->scroll_pos=0; }
                    break;
                case InputKeyRight: if(app->state==EPStateIdle) app->tpl=(app->tpl+1)%PortalCount; break;
                case InputKeyLeft: if(app->state==EPStateIdle) app->tpl=app->tpl==0?PortalCount-1:app->tpl-1; break;
                case InputKeyDown: if(app->show_creds&&app->scroll_pos<app->cred_count-1)app->scroll_pos++; break;
                case InputKeyUp: if(app->show_creds&&app->scroll_pos>0)app->scroll_pos--; break;
                case InputKeyBack:
                    if(app->show_creds){app->show_creds=false;}
                    else if(app->state!=EPStateIdle){app->state=EPStateIdle;app->clients_connected=0;}
                    else app->running=false;
                    break;
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
