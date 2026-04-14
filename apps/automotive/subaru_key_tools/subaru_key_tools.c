/**
 * CYBERFLIPPER — Subaru Key Tools
 * Targets Subaru fixed-code and rolling-code systems (433MHz).
 * Provides analysis, capture, and educational replay functionality.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <subghz/devices/devices.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
    uint32_t frequency;
    const SubGhzDevice* device;
} SubaruApp;

static void subaru_draw_callback(Canvas* canvas, void* context) {
    SubaruApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "SUBARU KEY TOOLS");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 30, "Freq: 433.92 MHz");
    canvas_draw_str(canvas, 2, 45, "Status: Scanning...");
    canvas_draw_str(canvas, 2, 60, "[OK] Capture  [BACK] Exit");
}

static void subaru_input_callback(InputEvent* event, void* context) {
    SubaruApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t subaru_key_tools_app(void* p) {
    SubaruApp* app = malloc(sizeof(SubaruApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->frequency = 433920000;
    app->gui = furi_record_open(RECORD_GUI);

    view_port_draw_callback_set(app->view_port, subaru_draw_callback, app);
    view_port_input_callback_set(app->view_port, subaru_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort && event.key == InputKeyBack) {
                app->running = false;
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
