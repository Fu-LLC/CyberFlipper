#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <storage/storage.h>

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "SPECTER: PASSIVE NODE");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "[ BLE + SubGHz Listen ]");
    canvas_draw_str(canvas, 2, 45, "Logging raw traffic...");
    canvas_draw_str(canvas, 2, 60, "Put deck in pocket.");
}

int32_t specter_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(uint32_t));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(1) {
        uint32_t event;
        if (furi_message_queue_get(event_queue, &event, 5000) == FuriStatusOk) break; 
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
    return 0;
}
