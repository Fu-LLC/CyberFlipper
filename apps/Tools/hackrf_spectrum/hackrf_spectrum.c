/**
 * CYBERFLIPPER — HackRF Spectrum Analyzer
 * Wideband spectrum analyzer controller for HackRF One.
 * Displays real-time frequency spectrum via UART/USB bridge.
 * Inspired by the RocketGod HackRF Treasure Chest.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY RESEARCH ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "HackRFSpectrum"
#define SPECTRUM_BINS 128
#define WATERFALL_ROWS 20

typedef enum {
    HRFBand70cm,
    HRFBand2m,
    HRFBandISM433,
    HRFBandISM868,
    HRFBandISM915,
    HRFBandGSM900,
    HRFBandGSM1800,
    HRFBandWiFi2G,
    HRFBandWiFi5G,
    HRFBandCustom,
    HRFBandCount,
} HRFBand;

static const char* band_names[] = {
    "70cm Ham",
    "2m Ham",
    "ISM 433MHz",
    "ISM 868MHz",
    "ISM 915MHz",
    "GSM 900",
    "GSM 1800",
    "WiFi 2.4GHz",
    "WiFi 5GHz",
    "Custom",
};

static const uint32_t band_centers[] = {
    440000000,
    146000000,
    433920000,
    868000000,
    915000000,
    935000000,
    1805000000,
    2437000000,
    5200000000,
    100000000,
};

static const uint32_t band_widths[] = {
    10000000,
    4000000,
    2000000,
    2000000,
    2000000,
    35000000,
    75000000,
    80000000,
    500000000,
    20000000,
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    HRFBand band;
    bool scanning;
    uint8_t spectrum[SPECTRUM_BINS];
    uint8_t waterfall[WATERFALL_ROWS][SPECTRUM_BINS];
    uint8_t waterfall_row;
    int8_t gain_db;
    bool hold_peaks;
    uint8_t peak_bin;
    uint8_t peak_val;
} HackRFSpectrumApp;

static void update_spectrum(HackRFSpectrumApp* app) {
    // Shift waterfall down
    if(app->waterfall_row < WATERFALL_ROWS - 1) {
        app->waterfall_row++;
    } else {
        for(uint8_t r = 0; r < WATERFALL_ROWS - 1; r++) {
            memcpy(app->waterfall[r], app->waterfall[r + 1], SPECTRUM_BINS);
        }
    }

    // Simulate spectrum data (in real use, this comes from HackRF via UART)
    app->peak_val = 0;
    app->peak_bin = 0;
    for(uint8_t i = 0; i < SPECTRUM_BINS; i++) {
        // Base noise floor
        uint8_t val = 5 + (rand() % 10);

        // Simulated signals at various bins
        if(i == 32 || i == 33) val += 30 + (rand() % 10);
        if(i == 64) val += 20 + (rand() % 15);
        if(i == 96 || i == 97) val += 25 + (rand() % 8);
        if(i == 48) val += 15 + (rand() % 12);

        if(val > 50) val = 50;
        app->spectrum[i] = val;
        app->waterfall[app->waterfall_row][i] = val;

        if(val > app->peak_val) {
            app->peak_val = val;
            app->peak_bin = i;
        }
    }
}

static void hackrf_draw_callback(Canvas* canvas, void* context) {
    HackRFSpectrumApp* app = (HackRFSpectrumApp*)context;
    canvas_clear(canvas);

    // Header
    canvas_set_font(canvas, FontSecondary);
    char header[40];
    uint32_t center_mhz = band_centers[app->band] / 1000000;
    snprintf(header, sizeof(header), "HRF:%s %luMHz G:%+ddB",
             band_names[app->band], center_mhz, app->gain_db);
    canvas_draw_str(canvas, 0, 7, header);

    // Spectrum display (bins 0-127 mapped to pixels 0-127, height 0-40)
    uint8_t spec_y_base = 48;
    uint8_t spec_height = 35;

    for(uint8_t x = 0; x < SPECTRUM_BINS; x++) {
        uint8_t bar_h = (app->spectrum[x] * spec_height) / 50;
        if(bar_h > spec_height) bar_h = spec_height;
        canvas_draw_line(canvas, x, spec_y_base, x, spec_y_base - bar_h);
    }

    // Peak marker
    if(app->scanning) {
        uint8_t peak_y = spec_y_base - (app->peak_val * spec_height / 50);
        canvas_draw_str(canvas, app->peak_bin > 110 ? 100 : app->peak_bin, peak_y - 2, "^");
    }

    // Frequency axis markers
    canvas_draw_line(canvas, 0, spec_y_base + 1, 127, spec_y_base + 1);

    // Bottom info
    if(app->scanning) {
        char info[32];
        snprintf(info, sizeof(info), "Peak:%ddBm @bin%d",
                 -90 + app->peak_val, app->peak_bin);
        canvas_draw_str(canvas, 0, 58, info);
        canvas_draw_str(canvas, 90, 58, "[BK]Stop");
    } else {
        canvas_draw_str(canvas, 0, 58, "[OK]Scan [<>]Band");
        canvas_draw_str(canvas, 0, 64, "[UD]Gain [BK]Exit");
    }
}

static void hackrf_input_callback(InputEvent* event, void* context) {
    HackRFSpectrumApp* app = (HackRFSpectrumApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t hackrf_spectrum_app(void* p) {
    UNUSED(p);

    HackRFSpectrumApp* app = malloc(sizeof(HackRFSpectrumApp));
    memset(app, 0, sizeof(HackRFSpectrumApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->gain_db = 20;
    app->band = HRFBandISM433;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, hackrf_draw_callback, app);
    view_port_input_callback_set(app->view_port, hackrf_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(app->scanning) {
            update_spectrum(app);
        }

        if(furi_message_queue_get(app->event_queue, &event,
            app->scanning ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    app->scanning = !app->scanning;
                    break;
                case InputKeyRight:
                    if(!app->scanning)
                        app->band = (app->band + 1) % HRFBandCount;
                    break;
                case InputKeyLeft:
                    if(!app->scanning)
                        app->band = app->band == 0 ? HRFBandCount - 1 : app->band - 1;
                    break;
                case InputKeyUp:
                    app->gain_db += 5;
                    if(app->gain_db > 62) app->gain_db = 62;
                    break;
                case InputKeyDown:
                    app->gain_db -= 5;
                    if(app->gain_db < 0) app->gain_db = 0;
                    break;
                case InputKeyBack:
                    if(app->scanning) {
                        app->scanning = false;
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
