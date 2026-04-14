/**
 * CYBERFLIPPER — Awesome Hacking Reference
 * Curated security toolkit reference card inspired by Awesome-Hacking.
 * Categories: Recon, Exploitation, Post-Exploit, Wireless, Web,
 * Forensics, Social Engineering, and Methodology.
 *
 * FOR EDUCATIONAL PURPOSES ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <string.h>

#define TAG "AwesomeHacking"

typedef enum {
    CatRecon,
    CatExploit,
    CatPostExploit,
    CatWireless,
    CatWeb,
    CatForensics,
    CatSocEng,
    CatMethodology,
    CatCount,
} HackCategory;

static const char* cat_names[] = {
    "Recon", "Exploit", "Post-Exploit", "Wireless",
    "Web", "Forensics", "Soc. Eng.", "Methodology",
};

// Recon tools
static const char* recon_items[] = {
    "nmap - Port scanner",
    "masscan - Fast scanner",
    "theHarvester - OSINT",
    "Shodan - IoT search",
    "Maltego - Intel graph",
    "Recon-ng - Framework",
    "Amass - Subdomain enum",
    "SpiderFoot - OSINT auto",
    "Censys - Internet scan",
    "Nikto - Web scanner",
};
#define RECON_COUNT 10

// Exploit tools
static const char* exploit_items[] = {
    "Metasploit - Framework",
    "Cobalt Strike - C2",
    "ExploitDB - Database",
    "pwntools - CTF/exploit",
    "ROPgadget - ROP chain",
    "commix - Cmd injection",
    "sqlmap - SQL injection",
    "BeEF - Browser exploit",
    "Responder - LLMNR/NBT",
    "CrackMapExec - AD tool",
};
#define EXPLOIT_COUNT 10

// Post-exploitation
static const char* postexploit_items[] = {
    "Mimikatz - Cred dump",
    "BloodHound - AD graph",
    "Empire - PowerShell C2",
    "Sliver - Cross-plat C2",
    "LinPEAS - Linux PE",
    "WinPEAS - Windows PE",
    "SharpHound - AD enum",
    "Rubeus - Kerberos",
    "Chisel - Tunnel/pivot",
    "Ligolo-ng - Tunneling",
};
#define POSTEXPLOIT_COUNT 10

// Wireless
static const char* wireless_items[] = {
    "aircrack-ng - WiFi crack",
    "Kismet - Wireless detect",
    "WiFi Pineapple - Rogue AP",
    "Bettercap - MITM/WiFi",
    "Fluxion - Evil twin",
    "Wifite - Auto WiFi atk",
    "HackRF - SDR platform",
    "RTL-SDR - RF receive",
    "GNURadio - SDR toolkit",
    "Ubertooth - BT sniff",
};
#define WIRELESS_COUNT 10

// Web
static const char* web_items[] = {
    "Burp Suite - Web proxy",
    "OWASP ZAP - Web scanner",
    "ffuf - Web fuzzer",
    "Gobuster - Dir brute",
    "WPScan - WordPress scan",
    "XSSHunter - XSS detect",
    "Nuclei - Vuln scanner",
    "httpx - HTTP probe",
    "Arjun - Param finder",
    "JWT_Tool - JWT attacks",
};
#define WEB_COUNT 10

// Forensics
static const char* forensics_items[] = {
    "Volatility - Memory",
    "Autopsy - Disk forensic",
    "Wireshark - Pcap analyst",
    "FTK - Forensic toolkit",
    "binwalk - Firmware anal",
    "foremost - File carve",
    "YARA - Pattern match",
    "Ghidra - Reverse eng",
    "radare2 - RE framework",
    "strings - Binary parse",
};
#define FORENSICS_COUNT 10

// Social Engineering
static const char* soceng_items[] = {
    "SET - Soc Eng Toolkit",
    "GoPhish - Phishing sim",
    "Evilginx - Phish proxy",
    "King Phisher - Campaign",
    "Catphish - Domain check",
    "Maltego - People intel",
    "BeEF - Browser hook",
    "PhoneInfoga - Phone OSINT",
    "theHarvester - Email",
    "SpiderFoot - OSINT auto",
};
#define SOCENG_COUNT 10

// Methodology
static const char* methodology_items[] = {
    "1. Pre-engagement",
    "2. Intelligence gather",
    "3. Threat modeling",
    "4. Vulnerability anal.",
    "5. Exploitation",
    "6. Post-exploitation",
    "7. Reporting",
    "PTES Standard",
    "OSSTMM v3",
    "NIST SP 800-115",
};
#define METHOD_COUNT 10

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    HackCategory category;
    uint8_t scroll_pos;
} AwesomeHackingApp;

static uint8_t get_count(HackCategory cat) {
    switch(cat) {
    case CatRecon: return RECON_COUNT;
    case CatExploit: return EXPLOIT_COUNT;
    case CatPostExploit: return POSTEXPLOIT_COUNT;
    case CatWireless: return WIRELESS_COUNT;
    case CatWeb: return WEB_COUNT;
    case CatForensics: return FORENSICS_COUNT;
    case CatSocEng: return SOCENG_COUNT;
    case CatMethodology: return METHOD_COUNT;
    default: return 0;
    }
}

static const char* get_item(HackCategory cat, uint8_t idx) {
    switch(cat) {
    case CatRecon: return recon_items[idx];
    case CatExploit: return exploit_items[idx];
    case CatPostExploit: return postexploit_items[idx];
    case CatWireless: return wireless_items[idx];
    case CatWeb: return web_items[idx];
    case CatForensics: return forensics_items[idx];
    case CatSocEng: return soceng_items[idx];
    case CatMethodology: return methodology_items[idx];
    default: return "";
    }
}

static void awesome_draw_callback(Canvas* canvas, void* context) {
    AwesomeHackingApp* app = (AwesomeHackingApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    char title[24];
    snprintf(title, sizeof(title), "AWESOME: %s", cat_names[app->category]);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, title);
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);
    uint8_t count = get_count(app->category);
    uint8_t visible = 5;

    for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < count; i++) {
        uint8_t idx = app->scroll_pos + i;
        char line[36];
        snprintf(line, sizeof(line), "%s%s",
                 idx == app->scroll_pos ? "> " : "  ",
                 get_item(app->category, idx));
        canvas_draw_str(canvas, 0, 19 + (i * 9), line);
    }

    // Category indicator
    char nav[40];
    snprintf(nav, sizeof(nav), "[<>]%s %u/%u", cat_names[app->category],
             app->scroll_pos + 1, count);
    canvas_draw_str(canvas, 2, 63, nav);
}

static void awesome_input_callback(InputEvent* event, void* context) {
    AwesomeHackingApp* app = (AwesomeHackingApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t awesome_hacking_app(void* p) {
    UNUSED(p);

    AwesomeHackingApp* app = malloc(sizeof(AwesomeHackingApp));
    memset(app, 0, sizeof(AwesomeHackingApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, awesome_draw_callback, app);
    view_port_input_callback_set(app->view_port, awesome_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                uint8_t count = get_count(app->category);
                switch(event.key) {
                case InputKeyDown:
                    if(app->scroll_pos < count - 1) app->scroll_pos++;
                    break;
                case InputKeyUp:
                    if(app->scroll_pos > 0) app->scroll_pos--;
                    break;
                case InputKeyRight:
                    app->category = (app->category + 1) % CatCount;
                    app->scroll_pos = 0;
                    break;
                case InputKeyLeft:
                    app->category = app->category == 0 ? CatCount - 1 : app->category - 1;
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
