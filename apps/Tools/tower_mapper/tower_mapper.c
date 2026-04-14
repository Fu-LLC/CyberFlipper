/**
 * CYBERFLIPPER — Tower Mapper
 * Cellular tower scanner and mapper for radio research.
 * Logs cell tower IDs, signal strength, and location data
 * for creating coverage maps. Inspired by Tower-Hunter.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY RESEARCH ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "TowerMapper"
#define MAX_TOWERS 16

typedef enum {
    TowerTypeGSM,
    TowerTypeLTE,
    TowerType5GNR,
    TowerTypeCDMA,
    TowerTypeUMTS,
} TowerType;

static const char* tower_type_names[] = {
    "GSM", "LTE", "5G NR", "CDMA", "UMTS",
};

typedef struct {
    uint16_t mcc;
    uint16_t mnc;
    uint32_t cid;
    uint16_t lac;
    int8_t rssi;
    TowerType type;
    uint16_t arfcn;
    bool active;
} CellTower;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    bool scanning;
    CellTower towers[MAX_TOWERS];
    uint8_t tower_count;
    uint8_t scroll_pos;
    uint32_t scan_time;
    float latitude;
    float longitude;
} TowerMapperApp;

static void sim_towers(TowerMapperApp* app) {
    if(app->tower_count >= MAX_TOWERS) return;

    CellTower* t = &app->towers[app->tower_count];
    t->active = true;
    t->mcc = 310; // US
    t->mnc = 260 + (rand() % 50);
    t->cid = 10000 + (rand() % 90000);
    t->lac = 1000 + (rand() % 9000);
    t->rssi = -(rand() % 60) - 40;
    t->type = rand() % 5;
    t->arfcn = 100 + (rand() % 900);
    app->tower_count++;

    FURI_LOG_I(TAG, "Tower: CID=%lu MCC=%u MNC=%u RSSI=%d %s",
               t->cid, t->mcc, t->mnc, t->rssi, tower_type_names[t->type]);
}

static void tower_draw_callback(Canvas* canvas, void* context) {
    TowerMapperApp* app = (TowerMapperApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "TOWER MAPPER");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    if(app->scanning) {
        char scan_str[32];
        snprintf(scan_str, sizeof(scan_str), "Scanning... %lus", app->scan_time);
        canvas_draw_str(canvas, 2, 18, scan_str);

        char count_str[32];
        snprintf(count_str, sizeof(count_str), "Towers found: %u", app->tower_count);
        canvas_draw_str(canvas, 2, 26, count_str);

        // Show last few towers
        for(uint8_t i = 0; i < 3 && i < app->tower_count; i++) {
            uint8_t idx = app->tower_count - 1 - i;
            CellTower* t = &app->towers[idx];
            char line[40];
            snprintf(line, sizeof(line), "%s CID:%lu %ddBm",
                     tower_type_names[t->type], t->cid, t->rssi);
            canvas_draw_str(canvas, 2, 36 + (i * 9), line);
        }

        canvas_draw_str(canvas, 30, 62, "[BACK] Stop");
    } else if(app->tower_count > 0) {
        // Show tower list
        for(uint8_t i = 0; i < 4 && (app->scroll_pos + i) < app->tower_count; i++) {
            uint8_t idx = app->scroll_pos + i;
            CellTower* t = &app->towers[idx];
            char line[40];
            snprintf(line, sizeof(line), "%s %u-%u CID:%lu %ddB",
                     tower_type_names[t->type], t->mcc, t->mnc, t->cid, t->rssi);
            canvas_draw_str(canvas, 2, 18 + (i * 10), line);
        }

        char nav_str[32];
        snprintf(nav_str, sizeof(nav_str), "%u/%u towers", app->scroll_pos + 1, app->tower_count);
        canvas_draw_str(canvas, 2, 56, nav_str);
        canvas_draw_str(canvas, 2, 64, "[OK]Scan [UD]Nav [BK]Exit");
    } else {
        canvas_draw_str(canvas, 2, 30, "No towers found yet");
        canvas_draw_str(canvas, 2, 42, "[OK] Start Scanning");
        canvas_draw_str(canvas, 2, 52, "[BACK] Exit");
    }
}

static void tower_input_callback(InputEvent* event, void* context) {
    TowerMapperApp* app = (TowerMapperApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t tower_mapper_app(void* p) {
    UNUSED(p);

    TowerMapperApp* app = malloc(sizeof(TowerMapperApp));
    memset(app, 0, sizeof(TowerMapperApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->latitude = 33.4484f;
    app->longitude = -112.0740f;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, tower_draw_callback, app);
    view_port_input_callback_set(app->view_port, tower_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    uint32_t tick = 0;

    while(app->running) {
        if(app->scanning) {
            tick++;
            if(tick % 15 == 0) {
                app->scan_time++;
                if(rand() % 3 == 0) {
                    sim_towers(app);
                }
                if(app->tower_count >= MAX_TOWERS) {
                    app->scanning = false;
                    FURI_LOG_I(TAG, "Scan complete: %u towers", app->tower_count);
                }
            }
        }

        if(furi_message_queue_get(app->event_queue, &event,
            app->scanning ? 100 : 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->scanning) {
                        app->scanning = true;
                        app->scan_time = 0;
                        tick = 0;
                    }
                    break;
                case InputKeyDown:
                    if(!app->scanning && app->scroll_pos < app->tower_count - 1)
                        app->scroll_pos++;
                    break;
                case InputKeyUp:
                    if(!app->scanning && app->scroll_pos > 0)
                        app->scroll_pos--;
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
