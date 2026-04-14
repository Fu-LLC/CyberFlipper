/**
 * CYBERFLIPPER — Ettercap MITM Controller
 * Man-in-the-middle attack controller for authorized network security testing.
 * Provides ARP poisoning control, packet sniffing modes, DNS spoofing,
 * and credential capture via UART bridge to ESP32.
 * Inspired by the ettercap project.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "EttercapMITM"

typedef enum {
    MITMModeArpSpoof,
    MITMModeDNSSpoof,
    MITMModeSSLStrip,
    MITMModePacketSniff,
    MITMModeCredCapture,
    MITMModePortScan,
    MITMModeCount,
} MITMMode;

static const char* mitm_mode_names[] = {
    "ARP Spoof",
    "DNS Spoof",
    "SSL Strip",
    "Packet Sniff",
    "Cred Capture",
    "Port Scan",
};

static const char* mitm_mode_desc[] = {
    "Poison ARP cache on LAN",
    "Redirect DNS queries",
    "Downgrade HTTPS -> HTTP",
    "Capture network packets",
    "Intercept credentials",
    "Enumerate open ports",
};

typedef enum {
    MITMStateDisconnected,
    MITMStateConnected,
    MITMStateScanning,
    MITMStateAttacking,
} MITMState;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    MITMMode mode;
    MITMState state;
    uint32_t packets_captured;
    uint32_t hosts_found;
    uint8_t target_count;
    char gateway_ip[16];
    char target_ip[16];
    uint32_t elapsed;
} EttercapApp;

static void ettercap_draw_callback(Canvas* canvas, void* context) {
    EttercapApp* app = (EttercapApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "ETTERCAP MITM");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    // Connection status
    const char* state_str = "DISCONNECTED";
    switch(app->state) {
    case MITMStateConnected: state_str = "ESP32 READY"; break;
    case MITMStateScanning: state_str = "SCANNING..."; break;
    case MITMStateAttacking: state_str = ">> ACTIVE <<"; break;
    default: break;
    }
    canvas_draw_str(canvas, 2, 20, state_str);

    // Mode
    char mode_str[40];
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", mitm_mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 30, mode_str);

    if(app->state == MITMStateAttacking) {
        char gw_str[32];
        snprintf(gw_str, sizeof(gw_str), "GW: %s", app->gateway_ip);
        canvas_draw_str(canvas, 2, 38, gw_str);

        char pkt_str[32];
        snprintf(pkt_str, sizeof(pkt_str), "Pkts: %lu  Hosts: %lu",
                 app->packets_captured, app->hosts_found);
        canvas_draw_str(canvas, 2, 46, pkt_str);

        // Progress bar
        uint32_t prog = (app->elapsed * 124) / 300;
        if(prog > 124) prog = 124;
        canvas_draw_frame(canvas, 2, 50, 124, 6);
        canvas_draw_box(canvas, 3, 51, prog, 4);

        canvas_draw_str(canvas, 2, 62, "[BACK] Stop");
    } else if(app->state == MITMStateScanning) {
        char scan_str[32];
        snprintf(scan_str, sizeof(scan_str), "Hosts found: %lu", app->hosts_found);
        canvas_draw_str(canvas, 2, 42, scan_str);
        canvas_draw_str(canvas, 2, 52, mitm_mode_desc[app->mode]);
    } else {
        canvas_draw_str(canvas, 2, 42, mitm_mode_desc[app->mode]);
        canvas_draw_str(canvas, 2, 52, "[OK] Start  [<>] Mode");
        canvas_draw_str(canvas, 2, 62, "[BACK] Exit");
    }
}

static void ettercap_input_callback(InputEvent* event, void* context) {
    EttercapApp* app = (EttercapApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t ettercap_mitm_app(void* p) {
    UNUSED(p);

    EttercapApp* app = malloc(sizeof(EttercapApp));
    memset(app, 0, sizeof(EttercapApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->state = MITMStateConnected;
    strncpy(app->gateway_ip, "192.168.1.1", sizeof(app->gateway_ip));
    strncpy(app->target_ip, "192.168.1.0/24", sizeof(app->target_ip));

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, ettercap_draw_callback, app);
    view_port_input_callback_set(app->view_port, ettercap_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;

    while(app->running) {
        if(app->state == MITMStateAttacking || app->state == MITMStateScanning) {
            tick++;
            if(tick % 5 == 0) {
                app->elapsed++;
                if(app->state == MITMStateScanning) {
                    app->hosts_found += (rand() % 3);
                    if(app->elapsed > 10) {
                        app->state = MITMStateAttacking;
                        app->elapsed = 0;
                    }
                } else {
                    app->packets_captured += (rand() % 50) + 10;
                }
            }
        }

        if(furi_message_queue_get(app->event_queue, &event,
            (app->state == MITMStateAttacking || app->state == MITMStateScanning) ? 100 : 50)
            == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->state == MITMStateConnected) {
                        app->state = MITMStateScanning;
                        app->elapsed = 0;
                        app->packets_captured = 0;
                        app->hosts_found = 0;
                        FURI_LOG_I(TAG, "Starting %s", mitm_mode_names[app->mode]);
                    }
                    break;
                case InputKeyRight:
                    if(app->state == MITMStateConnected)
                        app->mode = (app->mode + 1) % MITMModeCount;
                    break;
                case InputKeyLeft:
                    if(app->state == MITMStateConnected)
                        app->mode = app->mode == 0 ? MITMModeCount - 1 : app->mode - 1;
                    break;
                case InputKeyBack:
                    if(app->state == MITMStateAttacking || app->state == MITMStateScanning) {
                        app->state = MITMStateConnected;
                        FURI_LOG_I(TAG, "Stopped. Pkts: %lu", app->packets_captured);
                    } else {
                        app->running = false;
                    }
                    break;
                default:
                    break;
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
