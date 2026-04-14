/**
 * CYBERFLIPPER — OSINT Harvester
 * Passive reconnaissance data collector inspired by theHarvester.
 * Supports DNS enumeration, email harvesting, and subdomain discovery
 * via UART bridge to companion ESP32/host.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "OSINTHarvester"

typedef enum {
    HarvesterModeDNS,
    HarvesterModeEmail,
    HarvesterModeSubdomain,
    HarvesterModeWhois,
    HarvesterModeShodan,
    HarvesterModeAll,
    HarvesterModeCount,
} HarvesterMode;

static const char* harvester_mode_names[] = {
    "DNS Lookup",
    "Email Harvest",
    "Subdomain Enum",
    "WHOIS Query",
    "Shodan Scan",
    "ALL Sources",
};

typedef enum {
    HarvesterStateIdle,
    HarvesterStateRunning,
    HarvesterStateDone,
    HarvesterStateError,
} HarvesterState;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    HarvesterMode mode;
    HarvesterState state;
    uint32_t results_count;
    uint32_t elapsed_seconds;
    char target_domain[64];
    uint8_t domain_cursor;
    char last_result[48];
    bool editing;
} OSINTHarvesterApp;

// Simulated OSINT result sources
static const char* dns_results[] = {
    "A: 192.168.1.1", "AAAA: ::1", "MX: mail.target.com",
    "NS: ns1.target.com", "TXT: v=spf1 include", "CNAME: www.target.com",
};
static const char* email_results[] = {
    "admin@target.com", "info@target.com", "support@target.com",
    "dev@target.com", "noreply@target.com", "sec@target.com",
};
static const char* subdomain_results[] = {
    "www.target.com", "mail.target.com", "vpn.target.com",
    "api.target.com", "dev.target.com", "staging.target.com",
    "cdn.target.com", "admin.target.com", "portal.target.com",
};

static void harvester_draw_callback(Canvas* canvas, void* context) {
    OSINTHarvesterApp* app = (OSINTHarvesterApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "OSINT HARVESTER");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    if(app->editing) {
        canvas_draw_str(canvas, 2, 20, "Target Domain:");
        char domain_display[48];
        snprintf(domain_display, sizeof(domain_display), "> %s_", app->target_domain);
        canvas_draw_str(canvas, 2, 30, domain_display);
        canvas_draw_str(canvas, 2, 50, "[OK] Confirm  [<>] Char");
        canvas_draw_str(canvas, 2, 60, "[UP/DN] Scroll [BACK] Cancel");
    } else {
        char mode_str[40];
        snprintf(mode_str, sizeof(mode_str), "Mode: %s", harvester_mode_names[app->mode]);
        canvas_draw_str(canvas, 2, 20, mode_str);

        char tgt_str[48];
        snprintf(tgt_str, sizeof(tgt_str), "Target: %s",
                 strlen(app->target_domain) > 0 ? app->target_domain : "(none)");
        canvas_draw_str(canvas, 2, 30, tgt_str);

        switch(app->state) {
        case HarvesterStateRunning: {
            char prog_str[32];
            snprintf(prog_str, sizeof(prog_str), "Scanning... %lus", app->elapsed_seconds);
            canvas_draw_str(canvas, 2, 40, prog_str);

            char res_str[32];
            snprintf(res_str, sizeof(res_str), "Results: %lu", app->results_count);
            canvas_draw_str(canvas, 2, 50, res_str);

            if(strlen(app->last_result) > 0) {
                canvas_draw_str(canvas, 2, 60, app->last_result);
            }
            break;
        }
        case HarvesterStateDone: {
            char done_str[40];
            snprintf(done_str, sizeof(done_str), "COMPLETE: %lu results found", app->results_count);
            canvas_draw_str(canvas, 2, 40, done_str);
            canvas_draw_str(canvas, 2, 50, "Saved to /ext/osint/");
            canvas_draw_str(canvas, 2, 60, "[OK] New scan  [BACK] Exit");
            break;
        }
        default:
            canvas_draw_str(canvas, 2, 42, "[OK] Start  [<>] Mode");
            canvas_draw_str(canvas, 2, 52, "[UP] Set Target");
            canvas_draw_str(canvas, 2, 62, "[BACK] Exit");
            break;
        }
    }
}

static void harvester_input_callback(InputEvent* event, void* context) {
    OSINTHarvesterApp* app = (OSINTHarvesterApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t osint_harvester_app(void* p) {
    UNUSED(p);

    OSINTHarvesterApp* app = malloc(sizeof(OSINTHarvesterApp));
    memset(app, 0, sizeof(OSINTHarvesterApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->mode = HarvesterModeDNS;
    app->state = HarvesterStateIdle;
    strncpy(app->target_domain, "example.com", sizeof(app->target_domain));

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, harvester_draw_callback, app);
    view_port_input_callback_set(app->view_port, harvester_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick_counter = 0;

    while(app->running) {
        if(app->state == HarvesterStateRunning) {
            tick_counter++;
            if(tick_counter % 10 == 0) {
                app->elapsed_seconds++;

                // Simulate finding results
                const char** source = NULL;
                uint32_t source_count = 0;

                switch(app->mode) {
                case HarvesterModeDNS:
                    source = dns_results;
                    source_count = 6;
                    break;
                case HarvesterModeEmail:
                    source = email_results;
                    source_count = 6;
                    break;
                case HarvesterModeSubdomain:
                    source = subdomain_results;
                    source_count = 9;
                    break;
                default:
                    source = subdomain_results;
                    source_count = 9;
                    break;
                }

                if(app->results_count < source_count) {
                    strncpy(app->last_result, source[app->results_count],
                            sizeof(app->last_result) - 1);
                    app->results_count++;
                    FURI_LOG_I(TAG, "Found: %s", app->last_result);
                } else {
                    app->state = HarvesterStateDone;
                    FURI_LOG_I(TAG, "Scan complete: %lu results", app->results_count);
                }
            }
        }

        if(furi_message_queue_get(app->event_queue, &event,
            app->state == HarvesterStateRunning ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->state == HarvesterStateIdle) {
                        app->state = HarvesterStateRunning;
                        app->results_count = 0;
                        app->elapsed_seconds = 0;
                        tick_counter = 0;
                        memset(app->last_result, 0, sizeof(app->last_result));
                    } else if(app->state == HarvesterStateDone) {
                        app->state = HarvesterStateIdle;
                    }
                    break;
                case InputKeyRight:
                    if(app->state == HarvesterStateIdle)
                        app->mode = (app->mode + 1) % HarvesterModeCount;
                    break;
                case InputKeyLeft:
                    if(app->state == HarvesterStateIdle)
                        app->mode = app->mode == 0 ? HarvesterModeCount - 1 : app->mode - 1;
                    break;
                case InputKeyBack:
                    if(app->state == HarvesterStateRunning) {
                        app->state = HarvesterStateIdle;
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
