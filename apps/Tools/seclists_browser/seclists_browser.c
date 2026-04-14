/**
 * CYBERFLIPPER — SecLists Browser
 * On-device wordlist reference browser inspired by the SecLists project.
 * Built-in common passwords, usernames, directory paths, subdomains,
 * and fuzzing payloads for quick field reference.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <string.h>

#define TAG "SecListsBrowser"

typedef enum {
    ListPasswords,
    ListUsernames,
    ListDirectories,
    ListSubdomains,
    ListFuzzing,
    ListUserAgents,
    ListCount,
} ListCategory;

static const char* list_names[] = {
    "Passwords",
    "Usernames",
    "Directories",
    "Subdomains",
    "Fuzzing",
    "User-Agents",
};

// Top 25 passwords
static const char* passwords[] = {
    "123456", "password", "12345678", "qwerty", "123456789",
    "12345", "1234", "111111", "1234567", "dragon",
    "123123", "baseball", "abc123", "football", "monkey",
    "letmein", "shadow", "master", "666666", "qwertyuiop",
    "123321", "mustang", "michael", "654321", "superman",
};
#define PASSWORD_COUNT 25

// Common usernames
static const char* usernames[] = {
    "admin", "root", "user", "test", "guest",
    "info", "mysql", "operator", "ftpuser", "oracle",
    "postgres", "administrator", "backup", "www-data", "nobody",
    "sysadmin", "webmaster", "support", "service", "manager",
};
#define USERNAME_COUNT 20

// Common directories
static const char* directories[] = {
    "/admin/", "/login/", "/wp-admin/", "/api/", "/config/",
    "/.git/", "/.env", "/backup/", "/phpmyadmin/", "/console/",
    "/debug/", "/server-status", "/wp-login.php", "/robots.txt",
    "/.htaccess", "/sitemap.xml", "/swagger/", "/graphql",
    "/actuator/", "/.well-known/",
};
#define DIRECTORY_COUNT 20

// Common subdomains
static const char* subdomains[] = {
    "www", "mail", "ftp", "localhost", "webmail",
    "smtp", "pop", "ns1", "webdisk", "ns2",
    "cpanel", "whm", "autodiscover", "autoconfig", "m",
    "imap", "test", "ns", "blog", "pop3",
    "dev", "www2", "admin", "forum", "news",
    "vpn", "ns3", "mail2", "new", "mysql",
    "old", "portal", "api", "cdn", "staging",
};
#define SUBDOMAIN_COUNT 35

// Fuzzing payloads
static const char* fuzzing[] = {
    "' OR 1=1--", "<script>alert(1)</script>",
    "../../../etc/passwd", "%00", "{{7*7}}",
    "${7*7}", "AAAA%08x", "\\x41\\x41\\x41\\x41",
    "NULL", "undefined", "NaN", "true",
    "-1", "0", "99999999", "' AND '1'='1",
};
#define FUZZING_COUNT 16

// User agents
static const char* user_agents[] = {
    "Googlebot/2.1",
    "Mozilla/5.0 (compatible)",
    "curl/7.68.0",
    "sqlmap/1.4",
    "Nikto/2.1.6",
    "Nmap Scripting Engine",
    "DirBuster-1.0",
    "python-requests/2.25",
};
#define UA_COUNT 8

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    ListCategory category;
    uint8_t scroll_pos;
    uint32_t total_entries;
} SecListsApp;

static uint8_t get_list_count(ListCategory cat) {
    switch(cat) {
    case ListPasswords: return PASSWORD_COUNT;
    case ListUsernames: return USERNAME_COUNT;
    case ListDirectories: return DIRECTORY_COUNT;
    case ListSubdomains: return SUBDOMAIN_COUNT;
    case ListFuzzing: return FUZZING_COUNT;
    case ListUserAgents: return UA_COUNT;
    default: return 0;
    }
}

static const char* get_list_item(ListCategory cat, uint8_t idx) {
    switch(cat) {
    case ListPasswords: return passwords[idx];
    case ListUsernames: return usernames[idx];
    case ListDirectories: return directories[idx];
    case ListSubdomains: return subdomains[idx];
    case ListFuzzing: return fuzzing[idx];
    case ListUserAgents: return user_agents[idx];
    default: return "";
    }
}

static void seclists_draw_callback(Canvas* canvas, void* context) {
    SecListsApp* app = (SecListsApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    char title[32];
    snprintf(title, sizeof(title), "SECLISTS: %s", list_names[app->category]);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, title);
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);
    uint8_t count = get_list_count(app->category);
    uint8_t visible = 5;

    for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < count; i++) {
        char line[40];
        uint8_t idx = app->scroll_pos + i;
        if(idx == app->scroll_pos) {
            snprintf(line, sizeof(line), "> %s", get_list_item(app->category, idx));
        } else {
            snprintf(line, sizeof(line), "  %s", get_list_item(app->category, idx));
        }
        canvas_draw_str(canvas, 2, 19 + (i * 9), line);
    }

    // Scroll bar
    if(count > visible) {
        uint8_t bar_h = 42;
        uint8_t thumb_h = (bar_h * visible) / count;
        if(thumb_h < 3) thumb_h = 3;
        uint8_t thumb_y = 12 + (bar_h * app->scroll_pos) / count;
        canvas_draw_frame(canvas, 125, 12, 3, bar_h);
        canvas_draw_box(canvas, 125, thumb_y, 3, thumb_h);
    }

    char count_str[20];
    snprintf(count_str, sizeof(count_str), "%u/%u", app->scroll_pos + 1, count);
    canvas_draw_str(canvas, 90, 62, count_str);
    canvas_draw_str(canvas, 2, 62, "[<>]Cat [UD]Scroll");
}

static void seclists_input_callback(InputEvent* event, void* context) {
    SecListsApp* app = (SecListsApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t seclists_browser_app(void* p) {
    UNUSED(p);

    SecListsApp* app = malloc(sizeof(SecListsApp));
    memset(app, 0, sizeof(SecListsApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, seclists_draw_callback, app);
    view_port_input_callback_set(app->view_port, seclists_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                uint8_t count = get_list_count(app->category);
                switch(event.key) {
                case InputKeyDown:
                    if(app->scroll_pos < count - 1) app->scroll_pos++;
                    break;
                case InputKeyUp:
                    if(app->scroll_pos > 0) app->scroll_pos--;
                    break;
                case InputKeyRight:
                    app->category = (app->category + 1) % ListCount;
                    app->scroll_pos = 0;
                    break;
                case InputKeyLeft:
                    app->category = app->category == 0 ? ListCount - 1 : app->category - 1;
                    app->scroll_pos = 0;
                    break;
                case InputKeyOk:
                    FURI_LOG_I(TAG, "Selected: %s", get_list_item(app->category, app->scroll_pos));
                    break;
                case InputKeyBack:
                    app->running = false;
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
