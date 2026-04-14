#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <storage/storage.h>

#define WIGLE_HEADER "MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type\n"
#define LOG_FILE_PATH EXT_PATH("wardriver/wigle_net_log.csv")

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "CYBER_WARDRIVER: ACTIVE");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "[ WiFi + GPS Parsing ]");
    canvas_draw_str(canvas, 2, 35, "Format: WiGLE .CSV Map");
    canvas_draw_str(canvas, 2, 60, "Running in Pocket Mode ->");
}

int32_t wardriver_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(uint32_t));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, EXT_PATH("wardriver"));
    
    while(1) {
        uint32_t event;
        if (furi_message_queue_get(event_queue, &event, 2000) == FuriStatusOk) break; 
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    return 0;
}
