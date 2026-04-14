/**
 * CYBERFLIPPER — GNSS Analyzer
 * GPS/GNSS signal analyzer with coordinate spoofing capabilities.
 * Provides NMEA sentence parsing, satellite tracking, and
 * coordinate generation for security research.
 * Inspired by Ringmast4r/GNSS project.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY RESEARCH ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "GNSSAnalyzer"

typedef enum {
    GNSSViewLive,
    GNSSViewSatellites,
    GNSSViewSpoof,
    GNSSViewNMEA,
} GNSSView;

typedef struct {
    uint8_t prn;
    uint8_t elevation;
    uint16_t azimuth;
    uint8_t snr;
    bool in_use;
} SatInfo;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    GNSSView view;
    bool gps_active;

    // Position data
    float latitude;
    float longitude;
    float altitude;
    float speed_knots;
    uint8_t fix_quality;
    uint8_t sats_in_view;
    uint8_t sats_used;
    float hdop;

    // Satellites
    SatInfo satellites[12];

    // Spoof coordinates
    float spoof_lat;
    float spoof_lon;
    float spoof_alt;
    bool spoof_active;
    uint8_t spoof_cursor; // 0=lat, 1=lon, 2=alt

    // NMEA
    char nmea_sentence[80];
    uint8_t scroll_pos;
} GNSSApp;

// Predefined spoof locations
typedef struct {
    const char* name;
    float lat;
    float lon;
} SpoofPreset;

static const SpoofPreset spoof_presets[] = {
    {"White House", 38.8977f, -77.0365f},
    {"Pentagon", 38.8719f, -77.0563f},
    {"Area 51", 37.2350f, -115.8111f},
    {"Tokyo Tower", 35.6586f, 139.7454f},
    {"Eiffel Tower", 48.8584f, 2.2945f},
    {"Sydney Opera", -33.8568f, 151.2153f},
    {"Null Island", 0.0f, 0.0f},
    {"North Pole", 90.0f, 0.0f},
};
#define PRESET_COUNT 8

static void gen_nmea_gga(GNSSApp* app) {
    float lat = app->spoof_active ? app->spoof_lat : app->latitude;
    float lon = app->spoof_active ? app->spoof_lon : app->longitude;
    float alt = app->spoof_active ? app->spoof_alt : app->altitude;

    int lat_deg = (int)lat;
    float lat_min = (lat - lat_deg) * 60.0f;
    int lon_deg = (int)lon;
    float lon_min = (lon - lon_deg) * 60.0f;

    snprintf(app->nmea_sentence, sizeof(app->nmea_sentence),
             "$GPGGA,120000.00,%02d%07.4f,%c,%03d%07.4f,%c,%d,%02d,%.1f,%.1f,M,,M,,",
             lat_deg < 0 ? -lat_deg : lat_deg,
             lat_min < 0 ? -lat_min : lat_min,
             lat >= 0 ? 'N' : 'S',
             lon_deg < 0 ? -lon_deg : lon_deg,
             lon_min < 0 ? -lon_min : lon_min,
             lon >= 0 ? 'E' : 'W',
             app->fix_quality,
             app->sats_used,
             app->hdop,
             alt);
}

static void gnss_draw_callback(Canvas* canvas, void* context) {
    GNSSApp* app = (GNSSApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);

    switch(app->view) {
    case GNSSViewLive: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop,
            app->spoof_active ? "GNSS [SPOOFING]" : "GNSS ANALYZER");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);

        float lat = app->spoof_active ? app->spoof_lat : app->latitude;
        float lon = app->spoof_active ? app->spoof_lon : app->longitude;

        char lat_str[24];
        snprintf(lat_str, sizeof(lat_str), "Lat: %.6f", (double)lat);
        canvas_draw_str(canvas, 2, 20, lat_str);

        char lon_str[24];
        snprintf(lon_str, sizeof(lon_str), "Lon: %.6f", (double)lon);
        canvas_draw_str(canvas, 2, 28, lon_str);

        char alt_str[24];
        float alt = app->spoof_active ? app->spoof_alt : app->altitude;
        snprintf(alt_str, sizeof(alt_str), "Alt: %.1fm", (double)alt);
        canvas_draw_str(canvas, 2, 36, alt_str);

        char fix_str[32];
        snprintf(fix_str, sizeof(fix_str), "Fix:%d Sats:%d/%d HDOP:%.1f",
                 app->fix_quality, app->sats_used, app->sats_in_view, (double)app->hdop);
        canvas_draw_str(canvas, 2, 44, fix_str);

        canvas_draw_str(canvas, 2, 54, "[OK]Sats [>]Spoof [<]NMEA");
        canvas_draw_str(canvas, 2, 62, "[BACK] Exit");
        break;
    }
    case GNSSViewSatellites: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "SATELLITES");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);

        // Draw satellite bars
        for(uint8_t i = 0; i < 12 && i < app->sats_in_view; i++) {
            uint8_t x = 2 + (i * 10);
            uint8_t bar_h = app->satellites[i].snr;
            if(bar_h > 40) bar_h = 40;

            canvas_draw_frame(canvas, x, 52 - bar_h, 8, bar_h);
            if(app->satellites[i].in_use) {
                canvas_draw_box(canvas, x + 1, 53 - bar_h, 6, bar_h - 1);
            }

            char prn[4];
            snprintf(prn, sizeof(prn), "%d", app->satellites[i].prn);
            canvas_draw_str(canvas, x, 60, prn);
        }

        char info[32];
        snprintf(info, sizeof(info), "In view: %d  Used: %d",
                 app->sats_in_view, app->sats_used);
        canvas_draw_str(canvas, 2, 20, info);

        canvas_draw_str(canvas, 30, 64, "[BACK] Return");
        break;
    }
    case GNSSViewSpoof: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "GNSS SPOOF");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);

        char lat_str[24];
        snprintf(lat_str, sizeof(lat_str), "%sLat: %.4f",
                 app->spoof_cursor == 0 ? ">" : " ", (double)app->spoof_lat);
        canvas_draw_str(canvas, 2, 20, lat_str);

        char lon_str[24];
        snprintf(lon_str, sizeof(lon_str), "%sLon: %.4f",
                 app->spoof_cursor == 1 ? ">" : " ", (double)app->spoof_lon);
        canvas_draw_str(canvas, 2, 30, lon_str);

        char alt_str[24];
        snprintf(alt_str, sizeof(alt_str), "%sAlt: %.0fm",
                 app->spoof_cursor == 2 ? ">" : " ", (double)app->spoof_alt);
        canvas_draw_str(canvas, 2, 40, alt_str);

        canvas_draw_str(canvas, 2, 50,
            app->spoof_active ? "Status: ACTIVE" : "Status: OFF");

        canvas_draw_str(canvas, 2, 58, "[OK]Toggle [UD]Adj [<>]Sel");
        canvas_draw_str(canvas, 2, 64, "[BACK] Return");
        break;
    }
    case GNSSViewNMEA: {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "NMEA OUTPUT");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        gen_nmea_gga(app);

        // Display NMEA sentence wrapped
        char line1[28], line2[28], line3[28];
        strncpy(line1, app->nmea_sentence, 27);
        line1[27] = '\0';
        if(strlen(app->nmea_sentence) > 27) {
            strncpy(line2, app->nmea_sentence + 27, 27);
            line2[27] = '\0';
        } else {
            line2[0] = '\0';
        }
        if(strlen(app->nmea_sentence) > 54) {
            strncpy(line3, app->nmea_sentence + 54, 27);
            line3[27] = '\0';
        } else {
            line3[0] = '\0';
        }

        canvas_draw_str(canvas, 2, 20, line1);
        canvas_draw_str(canvas, 2, 30, line2);
        canvas_draw_str(canvas, 2, 40, line3);

        canvas_draw_str(canvas, 2, 54, "[OK] Send via UART");
        canvas_draw_str(canvas, 2, 62, "[BACK] Return");
        break;
    }
    }
}

static void gnss_input_callback(InputEvent* event, void* context) {
    GNSSApp* app = (GNSSApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t gnss_spoof_app(void* p) {
    UNUSED(p);

    GNSSApp* app = malloc(sizeof(GNSSApp));
    memset(app, 0, sizeof(GNSSApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    // Initialize with realistic demo data
    app->latitude = 33.4484f;
    app->longitude = -112.0740f;
    app->altitude = 331.0f;
    app->fix_quality = 1;
    app->sats_in_view = 10;
    app->sats_used = 7;
    app->hdop = 1.2f;
    app->spoof_lat = 38.8977f;
    app->spoof_lon = -77.0365f;
    app->spoof_alt = 17.0f;

    // Init satellites
    for(uint8_t i = 0; i < 12; i++) {
        app->satellites[i].prn = i + 1;
        app->satellites[i].snr = 15 + (rand() % 30);
        app->satellites[i].elevation = rand() % 90;
        app->satellites[i].azimuth = rand() % 360;
        app->satellites[i].in_use = i < app->sats_used;
    }

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, gnss_draw_callback, app);
    view_port_input_callback_set(app->view_port, gnss_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint8_t preset_idx = 0;

    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->view == GNSSViewLive) {
                        app->view = GNSSViewSatellites;
                    } else if(app->view == GNSSViewSpoof) {
                        app->spoof_active = !app->spoof_active;
                        FURI_LOG_I(TAG, "Spoof %s: %.4f, %.4f",
                            app->spoof_active ? "ON" : "OFF",
                            (double)app->spoof_lat, (double)app->spoof_lon);
                    } else if(app->view == GNSSViewNMEA) {
                        gen_nmea_gga(app);
                        FURI_LOG_I(TAG, "NMEA: %s", app->nmea_sentence);
                    }
                    break;
                case InputKeyRight:
                    if(app->view == GNSSViewLive) {
                        app->view = GNSSViewSpoof;
                    } else if(app->view == GNSSViewSpoof) {
                        // Cycle presets
                        preset_idx = (preset_idx + 1) % PRESET_COUNT;
                        app->spoof_lat = spoof_presets[preset_idx].lat;
                        app->spoof_lon = spoof_presets[preset_idx].lon;
                    }
                    break;
                case InputKeyLeft:
                    if(app->view == GNSSViewLive) {
                        app->view = GNSSViewNMEA;
                    } else if(app->view == GNSSViewSpoof) {
                        app->spoof_cursor = (app->spoof_cursor + 1) % 3;
                    }
                    break;
                case InputKeyUp:
                    if(app->view == GNSSViewSpoof) {
                        switch(app->spoof_cursor) {
                        case 0: app->spoof_lat += 0.01f; break;
                        case 1: app->spoof_lon += 0.01f; break;
                        case 2: app->spoof_alt += 10.0f; break;
                        }
                    }
                    break;
                case InputKeyDown:
                    if(app->view == GNSSViewSpoof) {
                        switch(app->spoof_cursor) {
                        case 0: app->spoof_lat -= 0.01f; break;
                        case 1: app->spoof_lon -= 0.01f; break;
                        case 2: app->spoof_alt -= 10.0f; break;
                        }
                    }
                    break;
                case InputKeyBack:
                    if(app->view != GNSSViewLive) {
                        app->view = GNSSViewLive;
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
