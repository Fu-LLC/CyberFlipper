/**
 * CYBERFLIPPER — Frequency Scanner
 * Rapidly scans all Sub-GHz frequency bands and highlights active
 * transmissions above a configurable RSSI threshold.
 * Displays top detected frequencies sorted by signal strength.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <subghz/devices/devices.h>
#include <furi_hal_region.h>
#include <stdlib.h>
#include <string.h>

#define TAG "FreqScanner"
#define MAX_DETECTIONS 10
#define RSSI_THRESHOLD_DEFAULT -65

static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 3,
    .bands = {
        {.start = 299999755, .end = 348000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 386999938, .end = 464000000, .power_limit = 20, .duty_cycle = 50},
        {.start = 778999847, .end = 928000000, .power_limit = 20, .duty_cycle = 50},
    },
};

typedef struct {
    uint32_t frequency;
    int8_t rssi;
    uint32_t last_seen;
    uint32_t hit_count;
} Detection;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    const SubGhzDevice* device;
    FuriThread* scan_thread;
    
    bool running;
    bool scanning;
    int8_t threshold;
    
    Detection detections[MAX_DETECTIONS];
    uint8_t detection_count;
    
    uint32_t scan_count;
    uint32_t current_freq;
    bool scan_active;
} FreqScannerApp;

static const uint32_t scan_frequencies[] = {
    300000000, 303875000, 304250000, 310000000, 312000000,
    313850000, 314350000, 315000000, 318000000,
    330000000, 345000000, 348000000,
    387000000, 390000000, 418000000,
    433075000, 433220000, 433420000, 433657000, 433868000,
    433920000, 434075000, 434175000, 434420000, 434620000, 434775000,
    438900000, 440175000,
    779000000, 868350000, 868400000, 868950000,
    902000000, 906000000, 910000000, 915000000, 920000000, 925000000,
};
#define NUM_SCAN_FREQS (sizeof(scan_frequencies) / sizeof(scan_frequencies[0]))

static void add_detection(FreqScannerApp* app, uint32_t freq, int8_t rssi) {
    // Check if already detected
    for(uint8_t i = 0; i < app->detection_count; i++) {
        if(app->detections[i].frequency == freq) {
            app->detections[i].rssi = rssi;
            app->detections[i].last_seen = furi_get_tick();
            app->detections[i].hit_count++;
            return;
        }
    }
    
    // Add new or replace weakest
    if(app->detection_count < MAX_DETECTIONS) {
        Detection* d = &app->detections[app->detection_count++];
        d->frequency = freq;
        d->rssi = rssi;
        d->last_seen = furi_get_tick();
        d->hit_count = 1;
    } else {
        // Replace weakest signal
        int8_t weakest = 0;
        uint8_t weakest_idx = 0;
        for(uint8_t i = 0; i < MAX_DETECTIONS; i++) {
            if(app->detections[i].rssi < app->detections[weakest_idx].rssi) {
                weakest_idx = i;
            }
        }
        if(rssi > app->detections[weakest_idx].rssi) {
            Detection* d = &app->detections[weakest_idx];
            d->frequency = freq;
            d->rssi = rssi;
            d->last_seen = furi_get_tick();
            d->hit_count = 1;
        }
    }
    
    // Sort by RSSI (strongest first)
    for(uint8_t i = 0; i < app->detection_count - 1; i++) {
        for(uint8_t j = i + 1; j < app->detection_count; j++) {
            if(app->detections[j].rssi > app->detections[i].rssi) {
                Detection tmp = app->detections[i];
                app->detections[i] = app->detections[j];
                app->detections[j] = tmp;
            }
        }
    }
}

static int32_t scanner_thread(void* context) {
    FreqScannerApp* app = context;
    
    while(app->scanning) {
        for(uint32_t i = 0; i < NUM_SCAN_FREQS && app->scanning; i++) {
            uint32_t freq = scan_frequencies[i];
            app->current_freq = freq;
            
            if(app->device) {
                subghz_devices_idle(app->device);
                subghz_devices_set_frequency(app->device, freq);
                subghz_devices_set_rx(app->device);
                furi_delay_ms(3);
                
                float rssi = subghz_devices_get_rssi(app->device);
                
                if((int8_t)rssi > app->threshold) {
                    add_detection(app, freq, (int8_t)rssi);
                }
                
                subghz_devices_idle(app->device);
            }
        }
        app->scan_count++;
    }
    
    return 0;
}

static void scanner_draw_callback(Canvas* canvas, void* context) {
    FreqScannerApp* app = (FreqScannerApp*)context;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBER FREQ SCANNER");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    
    canvas_set_font(canvas, FontSecondary);
    
    char info_str[40];
    snprintf(info_str, sizeof(info_str), "Scans:%lu Thr:%ddBm", 
             app->scan_count, app->threshold);
    canvas_draw_str(canvas, 2, 19, info_str);
    
    if(app->scan_active) {
        char scan_str[32];
        snprintf(scan_str, sizeof(scan_str), ">> %lu.%03lu MHz",
                 app->current_freq / 1000000,
                 (app->current_freq % 1000000) / 1000);
        canvas_draw_str(canvas, 68, 19, scan_str);
    }
    
    // Detection list
    uint8_t y = 28;
    uint8_t max_show = app->detection_count < 5 ? app->detection_count : 5;
    for(uint8_t i = 0; i < max_show; i++) {
        Detection* d = &app->detections[i];
        char det_str[48];
        snprintf(det_str, sizeof(det_str), "%lu.%03lu MHz  %ddBm  x%lu",
                 d->frequency / 1000000,
                 (d->frequency % 1000000) / 1000,
                 d->rssi,
                 d->hit_count);
        canvas_draw_str(canvas, 4, y, det_str);
        y += 8;
    }
    
    if(app->detection_count == 0) {
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "Scanning...");
    }
    
    canvas_draw_str(canvas, 2, 63, "[UP/DN]Thr [OK]Clear [BACK]Exit");
}

static void scanner_input_callback(InputEvent* input_event, void* context) {
    FreqScannerApp* app = (FreqScannerApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

int32_t freq_scanner_app(void* p) {
    UNUSED(p);
    
    FreqScannerApp* app = malloc(sizeof(FreqScannerApp));
    memset(app, 0, sizeof(FreqScannerApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->threshold = RSSI_THRESHOLD_DEFAULT;
    app->gui = furi_record_open(RECORD_GUI);
    
    furi_hal_region_set(&unlockedRegion);
    
    view_port_draw_callback_set(app->view_port, scanner_draw_callback, app);
    view_port_input_callback_set(app->view_port, scanner_input_callback, app);
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
        app->scan_active = true;
        app->scan_thread = furi_thread_alloc();
        furi_thread_set_name(app->scan_thread, "FreqScanner");
        furi_thread_set_stack_size(app->scan_thread, 2048);
        furi_thread_set_context(app->scan_thread, app);
        furi_thread_set_callback(app->scan_thread, scanner_thread);
        furi_thread_start(app->scan_thread);
    }
    
    furi_hal_power_suppress_charge_enter();
    
    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                    case InputKeyUp:
                        app->threshold += 5;
                        if(app->threshold > -20) app->threshold = -20;
                        break;
                    case InputKeyDown:
                        app->threshold -= 5;
                        if(app->threshold < -120) app->threshold = -120;
                        break;
                    case InputKeyOk:
                        app->detection_count = 0;
                        app->scan_count = 0;
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
