/**
 * CYBERFLIPPER — OWASP Cheat Sheet
 * On-device security reference cards inspired by OWASP CheatSheetSeries.
 * Quick-access reference for: OWASP Top 10, Common Ports, HTTP Status
 * Codes, SQL Injection patterns, XSS payloads, and password policies.
 *
 * FOR EDUCATIONAL PURPOSES ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <string.h>

#define TAG "CheatSheet"

typedef enum {
    SheetOWASPTop10,
    SheetCommonPorts,
    SheetHTTPCodes,
    SheetSQLInjection,
    SheetXSSPayloads,
    SheetLinuxCmds,
    SheetNetworkCmds,
    SheetHashTypes,
    SheetCount,
} SheetCategory;

static const char* category_names[] = {
    "OWASP Top 10",
    "Common Ports",
    "HTTP Codes",
    "SQL Injection",
    "XSS Payloads",
    "Linux Recon",
    "Network Cmds",
    "Hash Types",
};

// OWASP Top 10 (2021)
static const char* owasp_top10[] = {
    "A01: Broken Access Ctrl",
    "A02: Crypto Failures",
    "A03: Injection",
    "A04: Insecure Design",
    "A05: Security Misconfig",
    "A06: Vulnerable Components",
    "A07: Auth Failures",
    "A08: Data Integrity Fail",
    "A09: Logging Failures",
    "A10: SSRF",
};
#define OWASP_COUNT 10

// Common Ports
static const char* common_ports[] = {
    "21  FTP",
    "22  SSH",
    "23  Telnet",
    "25  SMTP",
    "53  DNS",
    "80  HTTP",
    "110 POP3",
    "135 MS-RPC",
    "139 NetBIOS",
    "443 HTTPS",
    "445 SMB",
    "1433 MSSQL",
    "1521 Oracle",
    "3306 MySQL",
    "3389 RDP",
    "5432 PostgreSQL",
    "5900 VNC",
    "6379 Redis",
    "8080 HTTP-Alt",
    "8443 HTTPS-Alt",
};
#define PORT_COUNT 20

// HTTP Status Codes
static const char* http_codes[] = {
    "200 OK",
    "201 Created",
    "301 Moved Permanently",
    "302 Found (Redirect)",
    "400 Bad Request",
    "401 Unauthorized",
    "403 Forbidden",
    "404 Not Found",
    "405 Method Not Allowed",
    "500 Internal Server Err",
    "502 Bad Gateway",
    "503 Service Unavailable",
};
#define HTTP_COUNT 12

// SQL Injection
static const char* sqli_payloads[] = {
    "' OR '1'='1",
    "' OR '1'='1' --",
    "' UNION SELECT NULL--",
    "1; DROP TABLE users--",
    "' AND 1=1 --",
    "' AND 1=2 --",
    "admin'--",
    "' ORDER BY 1--",
    "'; EXEC xp_cmdshell",
    "' WAITFOR DELAY '0:0:5",
};
#define SQLI_COUNT 10

// XSS Payloads
static const char* xss_payloads[] = {
    "<script>alert(1)</script>",
    "<img src=x onerror=alert(1)>",
    "<svg onload=alert(1)>",
    "javascript:alert(1)",
    "\"><script>alert(1)</script>",
    "';alert(1)//",
    "<iframe src=javascript:alert(1)",
    "<body onload=alert(1)>",
};
#define XSS_COUNT 8

// Linux Commands
static const char* linux_cmds[] = {
    "id; whoami",
    "uname -a",
    "cat /etc/passwd",
    "cat /etc/shadow",
    "netstat -tulpn",
    "ps aux",
    "find / -perm -4000",
    "ls -la /tmp",
    "env; printenv",
    "history",
};
#define LINUX_COUNT 10

// Network Commands
static const char* net_cmds[] = {
    "nmap -sV target",
    "nmap -sC -sV -O target",
    "nmap -p- target",
    "nmap --script=vuln target",
    "ping -c 4 target",
    "traceroute target",
    "dig target ANY",
    "whois target",
    "curl -I target",
    "nikto -h target",
};
#define NET_COUNT 10

// Hash Types
static const char* hash_types[] = {
    "MD5     32 hex chars",
    "SHA1    40 hex chars",
    "SHA256  64 hex chars",
    "SHA512  128 hex chars",
    "NTLM    32 hex chars",
    "bcrypt  $2a$...",
    "MySQL5  *41 hex chars",
    "DES     13 chars",
};
#define HASH_COUNT 8

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    SheetCategory category;
    uint8_t scroll_pos;
} CheatSheetApp;

static uint8_t get_item_count(SheetCategory cat) {
    switch(cat) {
    case SheetOWASPTop10: return OWASP_COUNT;
    case SheetCommonPorts: return PORT_COUNT;
    case SheetHTTPCodes: return HTTP_COUNT;
    case SheetSQLInjection: return SQLI_COUNT;
    case SheetXSSPayloads: return XSS_COUNT;
    case SheetLinuxCmds: return LINUX_COUNT;
    case SheetNetworkCmds: return NET_COUNT;
    case SheetHashTypes: return HASH_COUNT;
    default: return 0;
    }
}

static const char* get_item(SheetCategory cat, uint8_t idx) {
    switch(cat) {
    case SheetOWASPTop10: return owasp_top10[idx];
    case SheetCommonPorts: return common_ports[idx];
    case SheetHTTPCodes: return http_codes[idx];
    case SheetSQLInjection: return sqli_payloads[idx];
    case SheetXSSPayloads: return xss_payloads[idx];
    case SheetLinuxCmds: return linux_cmds[idx];
    case SheetNetworkCmds: return net_cmds[idx];
    case SheetHashTypes: return hash_types[idx];
    default: return "";
    }
}

static void cheat_draw_callback(Canvas* canvas, void* context) {
    CheatSheetApp* app = (CheatSheetApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, category_names[app->category]);
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);
    uint8_t count = get_item_count(app->category);
    uint8_t visible = 5;
    for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < count; i++) {
        canvas_draw_str(canvas, 2, 20 + (i * 9), get_item(app->category, app->scroll_pos + i));
    }

    // Scroll indicator
    if(count > visible) {
        uint8_t bar_h = 45;
        uint8_t thumb_h = (bar_h * visible) / count;
        uint8_t thumb_y = 12 + (bar_h * app->scroll_pos) / count;
        canvas_draw_frame(canvas, 125, 12, 3, bar_h);
        canvas_draw_box(canvas, 125, thumb_y, 3, thumb_h);
    }

    canvas_draw_str(canvas, 2, 63, "[<>] Category  [UP/DN] Scroll");
}

static void cheat_input_callback(InputEvent* event, void* context) {
    CheatSheetApp* app = (CheatSheetApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t cheat_sheet_app(void* p) {
    UNUSED(p);

    CheatSheetApp* app = malloc(sizeof(CheatSheetApp));
    memset(app, 0, sizeof(CheatSheetApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, cheat_draw_callback, app);
    view_port_input_callback_set(app->view_port, cheat_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                uint8_t count = get_item_count(app->category);
                switch(event.key) {
                case InputKeyDown:
                    if(app->scroll_pos < count - 1) app->scroll_pos++;
                    break;
                case InputKeyUp:
                    if(app->scroll_pos > 0) app->scroll_pos--;
                    break;
                case InputKeyRight:
                    app->category = (app->category + 1) % SheetCount;
                    app->scroll_pos = 0;
                    break;
                case InputKeyLeft:
                    app->category = app->category == 0 ? SheetCount - 1 : app->category - 1;
                    app->scroll_pos = 0;
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
