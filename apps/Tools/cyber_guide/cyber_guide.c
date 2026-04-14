/**
 * CYBERFLIPPER — Cyber Guide
 * Educational reference for security researchers.
 * Integrated logic from OWASP CheatSheetSeries.
 */

#include <furi.h>
#include <gui/gui.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    int page;
} GuideApp;

const char* guide_pages[] = {
    "OWASP: HARDWARE SECURITY\n1. Disable JTAG/UART\n2. Use Secure Boot\n3. Encrypt storage\n4. Remove default creds",
    "RF RESEARCH TIPS\n1. Verify pulse width\n2. Check for preamble\n3. Identify modulation\n4. Use waterfall view",
    "NETWORK DEFENSE\n1. Use MFA/2FA\n2. Segregate networks\n3. Monitor logs\n4. Audit permissions"
};

static void guide_draw_callback(Canvas* canvas, void* context) {
    GuideApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "CYBER RESEARCH GUIDE");
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_multiline(canvas, 2, 25, guide_pages[app->page]);
    
    canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "[<>] Pages   [BACK] Exit");
}

static void guide_input_callback(InputEvent* event, void* context) {
    GuideApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t cyber_guide_app(void* p) {
    GuideApp* app = malloc(sizeof(GuideApp));
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->running = true;
    app->page = 0;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, guide_draw_callback, app);
    view_port_input_callback_set(app->view_port, guide_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                if(event.key == InputKeyBack) {
                    app->running = false;
                } else if(event.key == InputKeyRight) {
                    app->page = (app->page + 1) % 3;
                } else if(event.key == InputKeyLeft) {
                    app->page = (app->page - 1 + 3) % 3;
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
