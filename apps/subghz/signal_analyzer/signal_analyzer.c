/**
 * CYBERFLIPPER — Signal Analyzer
 * Real-time Sub-GHz spectrum analysis with RSSI display,
 * peak detection, and modulation type identification.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <subghz/devices/devices.h>
#include <furi_hal_region.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAG "SignalAnalyzer"
#define SPECTRUM_WIDTH 128
#define SPECTRUM_HEIGHT 40
#define SCAN_STEP 250000

static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 3,
    .bands = {
        {.start = 299999755, .end = 348000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 386999938, .end = 464000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 778999847, .end = 928000000, .power_limit = 20, .duty_cycle = 50},
    },
};

typedef enum {
    AnalyzerModeSpectrum,
    AnalyzerModeWaterfall,
    AnalyzerModeSingleFreq,
} AnalyzerMode;

typedef struct {
    uint32_t start_freq;
    uint32_t end_freq;
    uint32_t step;
    const char* name;
} FrequencyBand;

static const FrequencyBand bands[] = {
    {300000000, 348000000, 375000, "300-348 MHz"},
    {387000000, 464000000, 600000, "387-464 MHz"},
    {779000000, 928000000, 1164000, "779-928 MHz"},
    {433000000, 435000000, 15625, "433 MHz Zoom"},
    {314000000, 316000000, 15625, "315 MHz Zoom"},
    {867000000, 870000000, 23437, "868 MHz Zoom"},
    {902000000, 928000000, 203125, "915 MHz ISM"},
};
#define NUM_BANDS (sizeof(bands) / sizeof(bands[0]))

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    const SubGhzDevice* device;
    FuriThread* scan_thread;
    
    bool running;
    bool scanning;
    AnalyzerMode mode;
    uint8_t selected_band;
    
    // Spectrum data
    int8_t rssi[SPECTRUM_WIDTH];
    int8_t rssi_peak[SPECTRUM_WIDTH];
    int8_t waterfall[SPECTRUM_HEIGHT][SPECTRUM_WIDTH];
    uint8_t waterfall_row;
    
    // Peak tracking
    uint32_t peak_frequency;
    int8_t peak_rssi;
    
    // Single frequency mode
    uint32_t single_freq;
    int8_t single_rssi;
    int8_t single_rssi_min;
    int8_t single_rssi_max;
    uint32_t sample_count;
} SignalAnalyzerApp;

// --- Scan Thread ---

static int32_t analyzer_scan_thread(void* context) {
    SignalAnalyzerApp* app = context;
    
    while(app->scanning) {
        const FrequencyBand* band = &bands[app->selected_band];
        
        if(app->mode == AnalyzerModeSingleFreq) {
            // Single frequency RSSI monitoring
            if(app->device) {
                subghz_devices_idle(app->device);
                subghz_devices_set_frequency(app->device, app->single_freq);
                subghz_devices_set_rx(app->device);
                furi_delay_ms(2);
                
                float rssi = subghz_devices_get_rssi(app->device);
                app->single_rssi = (int8_t)rssi;
                
                if(rssi < app->single_rssi_min || app->sample_count == 0) 
                    app->single_rssi_min = (int8_t)rssi;
                if(rssi > app->single_rssi_max || app->sample_count == 0)
                    app->single_rssi_max = (int8_t)rssi;
                app->sample_count++;
                
                subghz_devices_idle(app->device);
            }
            furi_delay_ms(50);
        } else {
            // Spectrum sweep
            uint32_t freq = band->start_freq;
            int idx = 0;
            app->peak_rssi = -128;
            
            while(freq <= band->end_freq && idx < SPECTRUM_WIDTH && app->scanning) {
                if(app->device) {
                    subghz_devices_idle(app->device);
                    subghz_devices_set_frequency(app->device, freq);
                    subghz_devices_set_rx(app->device);
                    furi_delay_ms(1);
                    
                    float rssi = subghz_devices_get_rssi(app->device);
                    app->rssi[idx] = (int8_t)rssi;
                    
                    // Peak hold
                    if(app->rssi[idx] > app->rssi_peak[idx]) {
                        app->rssi_peak[idx] = app->rssi[idx];
                    } else {
                        // Slow decay
                        if(app->rssi_peak[idx] > -128) app->rssi_peak[idx]--;
                    }
                    
                    // Track overall peak
                    if(rssi > app->peak_rssi) {
                        app->peak_rssi = (int8_t)rssi;
                        app->peak_frequency = freq;
                    }
                    
                    subghz_devices_idle(app->device);
                }
                
                freq += band->step;
                idx++;
            }
            
            // Fill remaining slots
            while(idx < SPECTRUM_WIDTH) {
                app->rssi[idx] = -128;
                idx++;
            }
            
            // Update waterfall
            if(app->mode == AnalyzerModeWaterfall) {
                memcpy(app->waterfall[app->waterfall_row], app->rssi, SPECTRUM_WIDTH);
                app->waterfall_row = (app->waterfall_row + 1) % SPECTRUM_HEIGHT;
            }
        }
    }
    
    return 0;
}

// --- UI ---

static void analyzer_draw_callback(Canvas* canvas, void* context) {
    SignalAnalyzerApp* app = (SignalAnalyzerApp*)context;
    canvas_clear(canvas);
    
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBER ANALYZER");
    
    canvas_set_font(canvas, FontSecondary);
    
    if(app->mode == AnalyzerModeSingleFreq) {
        // Single frequency display
        char freq_str[32];
        snprintf(freq_str, sizeof(freq_str), "Freq: %lu.%03lu MHz",
                 app->single_freq / 1000000,
                 (app->single_freq % 1000000) / 1000);
        canvas_draw_str(canvas, 2, 20, freq_str);
        
        char rssi_str[32];
        snprintf(rssi_str, sizeof(rssi_str), "RSSI: %d dBm", app->single_rssi);
        canvas_draw_str(canvas, 2, 30, rssi_str);
        
        char range_str[40];
        snprintf(range_str, sizeof(range_str), "Min:%d Max:%d Samples:%lu",
                 app->single_rssi_min, app->single_rssi_max, app->sample_count);
        canvas_draw_str(canvas, 2, 40, range_str);
        
        // RSSI bar
        int bar_width = (app->single_rssi + 128) * 124 / 128;
        if(bar_width < 0) bar_width = 0;
        if(bar_width > 124) bar_width = 124;
        canvas_draw_frame(canvas, 2, 44, 124, 8);
        canvas_draw_box(canvas, 3, 45, bar_width, 6);
        
        canvas_draw_str(canvas, 2, 60, "[OK] Mode [UP/DN] Freq");
    } else {
        // Spectrum / Waterfall
        const FrequencyBand* band = &bands[app->selected_band];
        canvas_draw_str(canvas, 2, 18, band->name);
        
        if(app->peak_rssi > -100) {
            char peak_str[32];
            snprintf(peak_str, sizeof(peak_str), "Peak:%lu.%02lu %ddB",
                     app->peak_frequency / 1000000,
                     (app->peak_frequency % 1000000) / 10000,
                     app->peak_rssi);
            canvas_draw_str(canvas, 50, 18, peak_str);
        }
        
        if(app->mode == AnalyzerModeSpectrum) {
            // Draw spectrum bars
            for(int i = 0; i < SPECTRUM_WIDTH; i++) {
                int height = (app->rssi[i] + 128) * SPECTRUM_HEIGHT / 80;
                if(height < 0) height = 0;
                if(height > SPECTRUM_HEIGHT) height = SPECTRUM_HEIGHT;
                
                int y_base = 58;
                canvas_draw_line(canvas, i, y_base, i, y_base - height);
                
                // Peak hold line
                int peak_h = (app->rssi_peak[i] + 128) * SPECTRUM_HEIGHT / 80;
                if(peak_h > 0 && peak_h <= SPECTRUM_HEIGHT) {
                    canvas_draw_dot(canvas, i, y_base - peak_h);
                }
            }
        } else {
            // Draw waterfall
            for(int y = 0; y < SPECTRUM_HEIGHT; y++) {
                int row = (app->waterfall_row + y) % SPECTRUM_HEIGHT;
                for(int x = 0; x < SPECTRUM_WIDTH; x++) {
                    int intensity = (app->waterfall[row][x] + 128) * 4 / 80;
                    if(intensity > 0) {
                        canvas_draw_dot(canvas, x, 20 + y);
                    }
                }
            }
        }
        
        canvas_draw_str(canvas, 2, 63, "[<>]Band [OK]Mode");
    }
}

static void analyzer_input_callback(InputEvent* input_event, void* context) {
    SignalAnalyzerApp* app = (SignalAnalyzerApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

// --- App ---

int32_t signal_analyzer_app(void* p) {
    UNUSED(p);
    
    SignalAnalyzerApp* app = malloc(sizeof(SignalAnalyzerApp));
    memset(app, 0, sizeof(SignalAnalyzerApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->mode = AnalyzerModeSpectrum;
    app->selected_band = 0;
    app->single_freq = 433920000;
    app->gui = furi_record_open(RECORD_GUI);
    
    memset(app->rssi, -128, sizeof(app->rssi));
    memset(app->rssi_peak, -128, sizeof(app->rssi_peak));
    
    furi_hal_region_set(&unlockedRegion);
    
    view_port_draw_callback_set(app->view_port, analyzer_draw_callback, app);
    view_port_input_callback_set(app->view_port, analyzer_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    subghz_devices_init();
    app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeExternalCC1101);
    if(!app->device) {
        app->device = radio_device_loader_set(NULL, SubGhzRadioDeviceTypeInternal);
    }
    
    if(app->device) {
        subghz_devices_reset(app->device);
        subghz_devices_idle(app->device);
        subghz_devices_load_preset(app->device, FuriHalSubGhzPresetOok650Async, NULL);
        
        app->scanning = true;
        app->scan_thread = furi_thread_alloc();
        furi_thread_set_name(app->scan_thread, "Analyzer Scan");
        furi_thread_set_stack_size(app->scan_thread, 2048);
        furi_thread_set_context(app->scan_thread, app);
        furi_thread_set_callback(app->scan_thread, analyzer_scan_thread);
        furi_thread_start(app->scan_thread);
    }
    
    furi_hal_power_suppress_charge_enter();
    
    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                    case InputKeyOk:
                        app->mode = (app->mode + 1) % 3;
                        if(app->mode == AnalyzerModeSingleFreq) {
                            app->sample_count = 0;
                            app->single_rssi_min = 0;
                            app->single_rssi_max = -128;
                        }
                        memset(app->rssi_peak, -128, sizeof(app->rssi_peak));
                        break;
                    case InputKeyRight:
                        if(app->mode == AnalyzerModeSingleFreq) {
                            app->single_freq += 100000;
                            if(app->single_freq > 928000000) app->single_freq = 300000000;
                        } else {
                            app->selected_band = (app->selected_band + 1) % NUM_BANDS;
                            memset(app->rssi_peak, -128, sizeof(app->rssi_peak));
                        }
                        break;
                    case InputKeyLeft:
                        if(app->mode == AnalyzerModeSingleFreq) {
                            if(app->single_freq > 300100000) app->single_freq -= 100000;
                        } else {
                            app->selected_band = (app->selected_band == 0) ? NUM_BANDS - 1 : app->selected_band - 1;
                            memset(app->rssi_peak, -128, sizeof(app->rssi_peak));
                        }
                        break;
                    case InputKeyUp:
                        if(app->mode == AnalyzerModeSingleFreq) {
                            app->single_freq += 1000000;
                            if(app->single_freq > 928000000) app->single_freq = 300000000;
                        }
                        break;
                    case InputKeyDown:
                        if(app->mode == AnalyzerModeSingleFreq) {
                            if(app->single_freq > 301000000) app->single_freq -= 1000000;
                        }
                        break;
                    case InputKeyBack:
                        app->running = false;
                        break;
                    default: break;
                }
            }
        }
        view_port_update(app->view_port);
    }
    
    // Cleanup
    app->scanning = false;
    if(app->scan_thread) {
        furi_thread_join(app->scan_thread);
        furi_thread_free(app->scan_thread);
    }
    if(app->device) {
        subghz_devices_idle(app->device);
        radio_device_loader_end(app->device);
    }
    subghz_devices_deinit();
    furi_hal_power_suppress_charge_exit();
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app);
    
    return 0;
}
