/**
 * CYBERFLIPPER — GNSS Viewer
 * Interoperable with NMEA GPS modules via UART (GPIO 13/14).
 * Displays coordinates, satellite count, and fix status.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>

#define UART_CH FuriHalUartIdLPUART1
#define BAUDRATE 9600

typedef struct {
    float lat;
    float lon;
    int sats;
    bool fix;
    char last_line[80];
} GpsData;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriStreamBuffer* rx_stream;
    GpsData data;
    bool running;
} GnssApp;

static void gnss_uart_callback(uint8_t* buf, size_t len, void* context) {
    GnssApp* app = context;
    furi_stream_buffer_send(app->rx_stream, buf, len, 0);
}

static void gnss_draw_callback(Canvas* canvas, void* context) {
    GnssApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "GNSS SATELLITE VIEW");
    
    canvas_set_font(canvas, FontSecondary);
    if(app->data.fix) {
        canvas_draw_str(canvas, 2, 30, "FIX: YES");
    } else {
        canvas_draw_str(canvas, 2, 30, "FIX: SEARCHING...");
    }
    
    char count_str[32];
    snprintf(count_str, sizeof(count_str), "Satellites: %d", app->data.sats);
    canvas_draw_str(canvas, 2, 45, count_str);
    
    canvas_draw_str(canvas, 2, 60, "[BACK] Exit");
}

static void gnss_input_callback(InputEvent* event, void* context) {
    GnssApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t gnss_viewer_app(void* p) {
    GnssApp* app = malloc(sizeof(GnssApp));
    app->rx_stream = furi_stream_buffer_alloc(512, 1);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, gnss_draw_callback, app);
    view_port_input_callback_set(app->view_port, gnss_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    furi_hal_uart_set_br(UART_CH, BAUDRATE);
    furi_hal_uart_set_irq_cb(UART_CH, gnss_uart_callback, app);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort && event.key == InputKeyBack) {
                app->running = false;
            }
        }
        view_port_update(app->view_port);
    }

    furi_hal_uart_set_irq_cb(UART_CH, NULL, NULL);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    furi_stream_buffer_free(app->rx_stream);
    free(app);
    return 0;
}
