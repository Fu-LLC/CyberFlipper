/**
 * CYBERFLIPPER — ProjectZero Vulnerability Browser
 * On-device CVE and vulnerability reference database.
 * Browse notable vulnerabilities by category with descriptions,
 * severity ratings, and remediation notes.
 * Inspired by Google Project Zero / C5Lab.
 *
 * FOR EDUCATIONAL PURPOSES ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <string.h>

#define TAG "ProjectZero"

typedef enum {
    VulnCatKernel,
    VulnCatWeb,
    VulnCatMobile,
    VulnCatNetwork,
    VulnCatIoT,
    VulnCatCrypto,
    VulnCatCount,
} VulnCategory;

static const char* vuln_cat_names[] = {
    "Kernel/OS",
    "Web/Browser",
    "Mobile",
    "Network",
    "IoT/Embedded",
    "Crypto",
};

typedef struct {
    const char* cve;
    const char* title;
    const char* severity;
} VulnEntry;

// Notable CVEs by category
static const VulnEntry kernel_vulns[] = {
    {"CVE-2024-1086", "nf_tables UAF priv escalation", "HIGH 7.8"},
    {"CVE-2023-32233", "Netfilter UAF arbitrary write", "HIGH 7.8"},
    {"CVE-2022-0847", "DirtyPipe local priv esc", "HIGH 7.8"},
    {"CVE-2021-4034", "Polkit pkexec priv esc", "HIGH 7.8"},
    {"CVE-2021-3156", "sudo heap overflow Baron Samedit", "HIGH 7.8"},
    {"CVE-2019-14287", "sudo user ID bypass", "HIGH 8.8"},
};

static const VulnEntry web_vulns[] = {
    {"CVE-2024-4577", "PHP-CGI arg injection RCE", "CRITICAL 9.8"},
    {"CVE-2023-44487", "HTTP/2 Rapid Reset DoS", "HIGH 7.5"},
    {"CVE-2021-44228", "Log4Shell RCE in Log4j", "CRITICAL 10.0"},
    {"CVE-2021-44832", "Log4j2 JDBC appender RCE", "MEDIUM 6.6"},
    {"CVE-2017-5638", "Apache Struts2 RCE", "CRITICAL 10.0"},
    {"CVE-2014-6271", "Shellshock Bash RCE", "CRITICAL 9.8"},
};

static const VulnEntry mobile_vulns[] = {
    {"CVE-2023-41064", "iOS BLASTPASS zero-click", "HIGH 7.8"},
    {"CVE-2023-32434", "iOS TriangulDB kernel RCE", "HIGH 7.8"},
    {"CVE-2021-1782", "iOS kernel race condition", "HIGH 7.0"},
    {"CVE-2020-3837", "iOS checkm8 bootrom", "HIGH 7.8"},
    {"CVE-2019-2215", "Android Binder UAF", "HIGH 7.8"},
    {"CVE-2017-13156", "Android Janus sig bypass", "HIGH 7.8"},
};

static const VulnEntry network_vulns[] = {
    {"CVE-2024-3400", "PAN-OS cmd inject zero-day", "CRITICAL 10.0"},
    {"CVE-2023-46805", "Ivanti Connect Secure auth bypass", "HIGH 8.2"},
    {"CVE-2023-20198", "Cisco IOS XE web UI priv esc", "CRITICAL 10.0"},
    {"CVE-2021-34527", "PrintNightmare RCE", "HIGH 8.8"},
    {"CVE-2020-1472", "Zerologon Netlogon priv esc", "CRITICAL 10.0"},
    {"CVE-2019-0708", "BlueKeep RDP RCE", "CRITICAL 9.8"},
};

static const VulnEntry iot_vulns[] = {
    {"CVE-2023-33466", "IP camera RCE via ONVIF", "CRITICAL 9.8"},
    {"CVE-2022-27255", "Realtek SDK stack overflow", "CRITICAL 9.8"},
    {"CVE-2021-35395", "Realtek Jungle SDK RCE", "CRITICAL 9.8"},
    {"CVE-2020-9054", "Zyxel NAS pre-auth RCE", "CRITICAL 9.8"},
    {"CVE-2018-10561", "DASAN GPON ONT auth bypass", "CRITICAL 9.8"},
    {"CVE-2016-10174", "Netgear R6250 stack overflow", "CRITICAL 9.8"},
};

static const VulnEntry crypto_vulns[] = {
    {"CVE-2023-38420", "OpenSSL X.509 policy crash", "HIGH 7.5"},
    {"CVE-2022-3602", "OpenSSL buffer overrun (Spooky)", "HIGH 7.5"},
    {"CVE-2014-0160", "Heartbleed OpenSSL read", "HIGH 7.5"},
    {"CVE-2017-15361", "ROCA (RSA key factoring)", "HIGH 7.5"},
    {"CVE-2016-0800", "DROWN SSLv2 attack", "HIGH 7.5"},
    {"CVE-2014-3566", "POODLE SSLv3 downgrade", "MEDIUM 4.3"},
};

#define ENTRIES_PER_CAT 6

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    VulnCategory category;
    uint8_t scroll_pos;
    bool detail_view;
} ProjectZeroApp;

static const VulnEntry* get_vulns(VulnCategory cat) {
    switch(cat) {
    case VulnCatKernel: return kernel_vulns;
    case VulnCatWeb: return web_vulns;
    case VulnCatMobile: return mobile_vulns;
    case VulnCatNetwork: return network_vulns;
    case VulnCatIoT: return iot_vulns;
    case VulnCatCrypto: return crypto_vulns;
    default: return kernel_vulns;
    }
}

static void pz_draw_callback(Canvas* canvas, void* context) {
    ProjectZeroApp* app = (ProjectZeroApp*)context;
    canvas_clear(canvas);

    const VulnEntry* vulns = get_vulns(app->category);

    canvas_set_font(canvas, FontPrimary);

    if(app->detail_view) {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CVE DETAIL");
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 20, vulns[app->scroll_pos].cve);
        canvas_draw_str(canvas, 2, 30, vulns[app->scroll_pos].title);
        canvas_draw_str(canvas, 2, 40, vulns[app->scroll_pos].severity);

        char cat_str[24];
        snprintf(cat_str, sizeof(cat_str), "Cat: %s", vuln_cat_names[app->category]);
        canvas_draw_str(canvas, 2, 50, cat_str);

        canvas_draw_str(canvas, 2, 62, "[BACK] Return");
    } else {
        char title[32];
        snprintf(title, sizeof(title), "P0: %s", vuln_cat_names[app->category]);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, title);
        canvas_draw_line(canvas, 0, 10, 128, 10);

        canvas_set_font(canvas, FontSecondary);
        for(uint8_t i = 0; i < 5 && i < ENTRIES_PER_CAT; i++) {
            uint8_t idx = i;
            char line[40];
            snprintf(line, sizeof(line), "%s%s %s",
                     idx == app->scroll_pos ? ">" : " ",
                     vulns[idx].cve, vulns[idx].severity);
            canvas_draw_str(canvas, 0, 19 + (i * 9), line);
        }

        canvas_draw_str(canvas, 2, 62, "[OK]Detail [<>]Cat [BK]Exit");
    }
}

static void pz_input_callback(InputEvent* event, void* context) {
    ProjectZeroApp* app = (ProjectZeroApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t project_zero_app(void* p) {
    UNUSED(p);

    ProjectZeroApp* app = malloc(sizeof(ProjectZeroApp));
    memset(app, 0, sizeof(ProjectZeroApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, pz_draw_callback, app);
    view_port_input_callback_set(app->view_port, pz_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    if(!app->detail_view) {
                        app->detail_view = true;
                    }
                    break;
                case InputKeyDown:
                    if(!app->detail_view && app->scroll_pos < ENTRIES_PER_CAT - 1)
                        app->scroll_pos++;
                    break;
                case InputKeyUp:
                    if(!app->detail_view && app->scroll_pos > 0)
                        app->scroll_pos--;
                    break;
                case InputKeyRight:
                    if(!app->detail_view) {
                        app->category = (app->category + 1) % VulnCatCount;
                        app->scroll_pos = 0;
                    }
                    break;
                case InputKeyLeft:
                    if(!app->detail_view) {
                        app->category = app->category == 0 ? VulnCatCount - 1 : app->category - 1;
                        app->scroll_pos = 0;
                    }
                    break;
                case InputKeyBack:
                    if(app->detail_view) {
                        app->detail_view = false;
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
