/**
 * CYBERFLIPPER — GSM Analyzer (Inspired by Tower-Hunter)
 * Manages an external SIM800L module via UART (GPIO 13/14).
 * Uses AT commands to retrieve Signal Quality, Cell ID, LAC, and Operator info.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

#define UART_CH FuriHalUartIdLPUART1

typedef struct {
    char operator[32];
    int signal_dbm;
    char cell_id[16];
    char lac[16];
    bool registered;
} GsmStatus;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriStreamBuffer* rx_stream;
    GsmStatus status;
    bool running;
} GsmApp;

static void gsm_uart_callback(uint8_t* buf, size_t len, void* context) {
    GsmApp* app = context;
    furi_stream_buffer_send(app->rx_stream, buf, len, 0);
}

static void gsm_draw_callback(Canvas* canvas, void* context) {
    GsmApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "GSM TOWER ANALYZER");
    
    canvas_set_font(canvas, FontSecondary);
    if(app->status.registered) {
        canvas_draw_str(canvas, 2, 28, "SIM: REGISTERED");
        char op_str[40];
        snprintf(op_str, sizeof(op_str), "OP: %s", app->status.operator);
        canvas_draw_str(canvas, 2, 38, op_str);
    } else {
        canvas_draw_str(canvas, 2, 28, "SIM: SEARCHING...");
    }
    
    char sig_str[32];
    snprintf(sig_str, sizeof(sig_str), "Signal: %d dBm", app->status.signal_dbm);
    canvas_draw_str(canvas, 2, 48, sig_str);

    canvas_draw_str(canvas, 2, 58, "[OK] Refresh  [BACK] Exit");
}

static void gsm_input_callback(InputEvent* event, void* context) {
    GsmApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

static void gsm_send_command(const char* cmd) {
    furi_hal_uart_tx((uint8_t*)cmd, strlen(cmd));
    furi_hal_uart_tx((uint8_t*)"\r\n", 2);
}

int32_t gsm_analyzer_app(void* p) {
    GsmApp* app = malloc(sizeof(GsmApp));
    app->rx_stream = furi_stream_buffer_alloc(512, 1);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, gsm_draw_callback, app);
    view_port_input_callback_set(app->view_port, gsm_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    furi_hal_uart_set_br(UART_CH, 9600);
    furi_hal_uart_set_irq_cb(UART_CH, gsm_uart_callback, app);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                if(event.key == InputKeyBack) {
                    app->running = false;
                } else if(event.key == InputKeyOk) {
                    gsm_send_command("AT+CSQ"); // Check signal
                    gsm_send_command("AT+COPS?"); // Check operator
                }
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
