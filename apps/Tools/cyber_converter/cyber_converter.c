/**
 * CYBERFLIPPER — Cyber Converter
 * Real-time data conversion tool for field researchers.
 * Inspired by the logic of CyberChef.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

typedef struct {
    uint32_t value;
    uint8_t cursor;
} ConverterData;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    ConverterData data;
    bool running;
} CyberConvApp;

static void conv_draw_callback(Canvas* canvas, void* context) {
    CyberConvApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "CYBER CONVERTER");
    
    canvas_set_font(canvas, FontSecondary);
    char hex_str[32];
    snprintf(hex_str, sizeof(hex_str), "HEX: 0x%08lX", app->data.value);
    canvas_draw_str(canvas, 2, 28, hex_str);

    char dec_str[32];
    snprintf(dec_str, sizeof(dec_str), "DEC: %lu", app->data.value);
    canvas_draw_str(canvas, 2, 38, dec_str);

    char bin_str[40];
    snprintf(bin_str, sizeof(bin_str), "BIN: %08b", (uint8_t)(app->data.value & 0xFF));
    canvas_draw_str(canvas, 2, 48, bin_str);
    
    canvas_draw_str(canvas, 2, 60, "[<>] Edit   [BACK] Exit");
}

static void conv_input_callback(InputEvent* event, void* context) {
    CyberConvApp* app = context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t cyber_converter_app(void* p) {
    CyberConvApp* app = malloc(sizeof(CyberConvApp));
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->running = true;
    app->data.value = 0x41; // 'A'

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, conv_draw_callback, app);
    view_port_input_callback_set(app->view_port, conv_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                if(event.key == InputKeyBack) {
                    app->running = false;
                } else if(event.key == InputKeyUp) {
                    app->data.value++;
                } else if(event.key == InputKeyDown) {
                    app->data.value--;
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
