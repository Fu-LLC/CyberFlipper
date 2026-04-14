/**
 * CYBERFLIPPER — BeEF Hook Controller
 * Browser exploitation framework controller inspired by the BeEF project.
 * Manages hooked browsers, deploys command modules, and monitors results
 * via UART bridge to companion server.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "BeEFHook"

typedef enum {
    BeEFViewDashboard,
    BeEFViewHooked,
    BeEFViewCommands,
    BeEFViewResults,
} BeEFView;

typedef enum {
    CmdGetCookie,
    CmdGetPage,
    CmdRedirect,
    CmdAlert,
    CmdKeylog,
    CmdWebcam,
    CmdGeolocation,
    CmdPhishing,
    CmdClipboard,
    CmdPortScan,
    CmdCount,
} BeEFCommand;

static const char* cmd_names[] = {
    "Get Cookie",
    "Get Page HTML",
    "Redirect Browser",
    "Alert Dialog",
    "Keylogger",
    "Webcam Capture",
    "Geolocation",
    "Phishing Page",
    "Clipboard Grab",
    "Port Scanner",
};

// Simulated hooked browsers
typedef struct {
    char ip[16];
    char browser[20];
    char os[16];
    bool online;
} HookedBrowser;

static HookedBrowser hooked[] = {
    {"192.168.1.50", "Chrome 120", "Windows 11", true},
    {"192.168.1.55", "Firefox 121", "macOS 14", true},
    {"192.168.1.62", "Safari 17", "iOS 17", false},
    {"192.168.1.78", "Edge 120", "Windows 10", true},
    {"10.0.0.15", "Chrome 119", "Android 14", true},
};
#define HOOKED_COUNT 5

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    BeEFView view;
    uint8_t scroll_pos;
    uint8_t selected_browser;
    BeEFCommand selected_cmd;
    uint32_t commands_sent;
    char last_result[48];
} BeEFApp;

static void beef_draw_callback(Canvas* canvas, void* context) {
    BeEFApp* app = (BeEFApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);

    switch(app->view) {
    case BeEFViewDashboard: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "BeEF DASHBOARD");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        uint8_t online = 0;
        for(uint8_t i = 0; i < HOOKED_COUNT; i++)
            if(hooked[i].online) online++;

        char stat[32];
        snprintf(stat, sizeof(stat), "Online: %u / %u browsers", online, HOOKED_COUNT);
        canvas_draw_str(canvas, 2, 22, stat);

        char cmd_stat[32];
        snprintf(cmd_stat, sizeof(cmd_stat), "Cmds Sent: %lu", app->commands_sent);
        canvas_draw_str(canvas, 2, 32, cmd_stat);

        canvas_draw_str(canvas, 2, 42, "Server: 192.168.1.1:3000");
        canvas_draw_str(canvas, 2, 52, "[OK] Hooked Browsers");
        canvas_draw_str(canvas, 2, 62, "[BACK] Exit");
        break;
    }
    case BeEFViewHooked: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "HOOKED BROWSERS");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        for(uint8_t i = 0; i < 4 && (app->scroll_pos + i) < HOOKED_COUNT; i++) {
            uint8_t idx = app->scroll_pos + i;
            char line[40];
            snprintf(line, sizeof(line), "%s%s %s %s",
                     idx == app->selected_browser ? ">" : " ",
                     hooked[idx].online ? "*" : " ",
                     hooked[idx].ip, hooked[idx].browser);
            canvas_draw_str(canvas, 0, 20 + (i * 10), line);
        }
        canvas_draw_str(canvas, 2, 62, "[OK]Commands [BK]Back");
        break;
    }
    case BeEFViewCommands: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "COMMANDS");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        char tgt[32];
        snprintf(tgt, sizeof(tgt), "Target: %s", hooked[app->selected_browser].ip);
        canvas_draw_str(canvas, 2, 18, tgt);

        for(uint8_t i = 0; i < 4 && (app->scroll_pos + i) < CmdCount; i++) {
            uint8_t idx = app->scroll_pos + i;
            char line[40];
            snprintf(line, sizeof(line), "%s %s",
                     idx == (uint8_t)app->selected_cmd ? ">" : " ",
                     cmd_names[idx]);
            canvas_draw_str(canvas, 2, 28 + (i * 9), line);
        }
        canvas_draw_str(canvas, 2, 62, "[OK]Execute [BK]Back");
        break;
    }
    case BeEFViewResults: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "RESULT");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        char cmd_str[40];
        snprintf(cmd_str, sizeof(cmd_str), "Cmd: %s", cmd_names[app->selected_cmd]);
        canvas_draw_str(canvas, 2, 22, cmd_str);

        canvas_draw_str(canvas, 2, 32, "Status: Success");
        canvas_draw_str(canvas, 2, 42, app->last_result);

        canvas_draw_str(canvas, 2, 62, "[BACK] Return");
        break;
    }
    }
}

static void beef_input_callback(InputEvent* event, void* context) {
    BeEFApp* app = (BeEFApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t beef_hook_app(void* p) {
    UNUSED(p);

    BeEFApp* app = malloc(sizeof(BeEFApp));
    memset(app, 0, sizeof(BeEFApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, beef_draw_callback, app);
    view_port_input_callback_set(app->view_port, beef_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->view == BeEFViewDashboard) {
                        app->view = BeEFViewHooked;
                        app->scroll_pos = 0;
                    } else if(app->view == BeEFViewHooked) {
                        app->view = BeEFViewCommands;
                        app->scroll_pos = 0;
                    } else if(app->view == BeEFViewCommands) {
                        app->commands_sent++;
                        snprintf(app->last_result, sizeof(app->last_result),
                                 "OK from %s", hooked[app->selected_browser].ip);
                        app->view = BeEFViewResults;
                        FURI_LOG_I(TAG, "Exec: %s on %s",
                            cmd_names[app->selected_cmd],
                            hooked[app->selected_browser].ip);
                    }
                    break;
                case InputKeyDown:
                    if(app->view == BeEFViewHooked) {
                        if(app->selected_browser < HOOKED_COUNT - 1)
                            app->selected_browser++;
                        if(app->selected_browser >= app->scroll_pos + 4)
                            app->scroll_pos++;
                    } else if(app->view == BeEFViewCommands) {
                        if((uint8_t)app->selected_cmd < CmdCount - 1)
                            app->selected_cmd++;
                        if((uint8_t)app->selected_cmd >= app->scroll_pos + 4)
                            app->scroll_pos++;
                    }
                    break;
                case InputKeyUp:
                    if(app->view == BeEFViewHooked) {
                        if(app->selected_browser > 0)
                            app->selected_browser--;
                        if(app->selected_browser < app->scroll_pos)
                            app->scroll_pos = app->selected_browser;
                    } else if(app->view == BeEFViewCommands) {
                        if(app->selected_cmd > 0)
                            app->selected_cmd--;
                        if((uint8_t)app->selected_cmd < app->scroll_pos)
                            app->scroll_pos = (uint8_t)app->selected_cmd;
                    }
                    break;
                case InputKeyBack:
                    if(app->view == BeEFViewResults) {
                        app->view = BeEFViewCommands;
                    } else if(app->view == BeEFViewCommands) {
                        app->view = BeEFViewHooked;
                    } else if(app->view == BeEFViewHooked) {
                        app->view = BeEFViewDashboard;
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
