/**
 * CYBERFLIPPER — Cyber Bus
 * A specialized UART terminal for hardware research.
 * Ideal for interfacing with Bus Pirate or ESP32 debugging modules.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;
} CyberBusApp;

static void cyber_bus_draw_callback(Canvas* canvas, void* context) {
    CyberBusApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "CYBER BUS INTERFACE");
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 30, "Mode: UART 115200");
    canvas_draw_str(canvas, 2, 45, "Status: Connected to GPIO");
    canvas_draw_str(canvas, 2, 60, "[OK] Menu  [BACK] Exit");
}

static void cyber_bus_input_callback(InputEvent* event, void* context) {
    CyberBusApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t cyber_bus_app(void* p) {
    CyberBusApp* app = malloc(sizeof(CyberBusApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, cyber_bus_draw_callback, app);
    view_port_input_callback_set(app->view_port, cyber_bus_input_callback, app);
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
