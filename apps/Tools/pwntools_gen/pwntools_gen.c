/**
 * CYBERFLIPPER — Pwntools Generator
 * Exploit development pattern generator inspired by pwntools.
 * Generates cyclic patterns for buffer overflow testing, provides
 * common ROP gadget references, and shellcode templates.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "PwntoolsGen"

typedef enum {
    PwnModeCyclic,
    PwnModeOffset,
    PwnModeShellcode,
    PwnModeROPGadgets,
    PwnModeFormat,
    PwnModePacking,
    PwnModeCount,
} PwnMode;

static const char* pwn_mode_names[] = {
    "Cyclic Pattern",
    "Offset Finder",
    "Shellcode Ref",
    "ROP Gadgets",
    "Format String",
    "Pack/Unpack",
};

// Shellcode references
static const char* shellcode_refs[] = {
    "Linux/x86 execve /bin/sh",
    "  \\x31\\xc0\\x50\\x68...",
    "  23 bytes",
    "",
    "Linux/x64 execve /bin/sh",
    "  \\x48\\x31\\xff\\x48...",
    "  27 bytes",
    "",
    "Windows/x86 WinExec calc",
    "  \\x31\\xc9\\x64\\x8b...",
    "  195 bytes",
};
#define SHELLCODE_LINES 11

// ROP Gadgets
static const char* rop_gadgets[] = {
    "pop rdi; ret",
    "pop rsi; ret",
    "pop rdx; ret",
    "pop rcx; ret",
    "pop rax; ret",
    "syscall; ret",
    "ret (align stack)",
    "leave; ret",
    "xchg eax,esp; ret",
    "mov [rdi],rsi; ret",
};
#define ROP_COUNT 10

// Format string attacks
static const char* fmt_strings[] = {
    "%x.%x.%x.%x",
    "%n%n%n%n",
    "%08x.%08x.%08x",
    "%s%s%s%s",
    "AAAA%08x.%08x.%n",
    "%1$n (direct param)",
    "%7$x (stack offset)",
};
#define FMT_COUNT 7

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    PwnMode mode;
    uint16_t pattern_len;
    uint8_t scroll_pos;
    char cyclic_buf[64];
} PwntoolsApp;

// Generate De Bruijn-like cyclic pattern
static void generate_cyclic(char* output, uint16_t len) {
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char lower[] = "abcdefghijklmnopqrstuvwxyz";
    const char digits[] = "0123456789";
    uint16_t pos = 0;

    for(uint8_t a = 0; a < 26 && pos < len; a++) {
        for(uint8_t b = 0; b < 26 && pos < len; b++) {
            for(uint8_t c = 0; c < 10 && pos < len; c++) {
                if(pos < len) output[pos++] = upper[a];
                if(pos < len) output[pos++] = lower[b];
                if(pos < len) output[pos++] = digits[c];
            }
        }
    }
    if(len < 64) output[len] = '\0';
    else output[63] = '\0';
}

static void pwn_draw_callback(Canvas* canvas, void* context) {
    PwntoolsApp* app = (PwntoolsApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "PWNTOOLS GEN");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    char mode_str[32];
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", pwn_mode_names[app->mode]);
    canvas_draw_str(canvas, 2, 20, mode_str);

    switch(app->mode) {
    case PwnModeCyclic: {
        char len_str[32];
        snprintf(len_str, sizeof(len_str), "Length: %u bytes", app->pattern_len);
        canvas_draw_str(canvas, 2, 30, len_str);

        // Show first 30 chars of pattern
        char display[32];
        strncpy(display, app->cyclic_buf, 30);
        display[30] = '\0';
        canvas_draw_str(canvas, 2, 40, display);

        canvas_draw_str(canvas, 2, 52, "[OK] Generate [UP/DN] Size");
        break;
    }
    case PwnModeShellcode: {
        uint8_t visible = 4;
        for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < SHELLCODE_LINES; i++) {
            canvas_draw_str(canvas, 2, 30 + (i * 9), shellcode_refs[app->scroll_pos + i]);
        }
        canvas_draw_str(canvas, 2, 62, "[UP/DN] Scroll");
        break;
    }
    case PwnModeROPGadgets: {
        uint8_t visible = 4;
        for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < ROP_COUNT; i++) {
            char line[40];
            snprintf(line, sizeof(line), "%d: %s", app->scroll_pos + i + 1,
                     rop_gadgets[app->scroll_pos + i]);
            canvas_draw_str(canvas, 2, 30 + (i * 9), line);
        }
        canvas_draw_str(canvas, 2, 62, "[UP/DN] Scroll");
        break;
    }
    case PwnModeFormat: {
        uint8_t visible = 4;
        for(uint8_t i = 0; i < visible && (app->scroll_pos + i) < FMT_COUNT; i++) {
            canvas_draw_str(canvas, 2, 30 + (i * 9), fmt_strings[app->scroll_pos + i]);
        }
        canvas_draw_str(canvas, 2, 62, "[UP/DN] Scroll");
        break;
    }
    case PwnModePacking: {
        canvas_draw_str(canvas, 2, 30, "p32(0xdeadbeef)");
        canvas_draw_str(canvas, 6, 40, "\\xef\\xbe\\xad\\xde");
        canvas_draw_str(canvas, 2, 50, "p64(0x4141414141414141)");
        canvas_draw_str(canvas, 6, 60, "\\x41\\x41\\x41\\x41...");
        break;
    }
    default:
        canvas_draw_str(canvas, 2, 40, "[OK] to interact");
        break;
    }
}

static void pwn_input_callback(InputEvent* event, void* context) {
    PwntoolsApp* app = (PwntoolsApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t pwntools_gen_app(void* p) {
    UNUSED(p);

    PwntoolsApp* app = malloc(sizeof(PwntoolsApp));
    memset(app, 0, sizeof(PwntoolsApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    app->pattern_len = 32;
    generate_cyclic(app->cyclic_buf, 63);

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, pwn_draw_callback, app);
    view_port_input_callback_set(app->view_port, pwn_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                switch(event.key) {
                case InputKeyOk:
                    if(app->mode == PwnModeCyclic) {
                        generate_cyclic(app->cyclic_buf, app->pattern_len > 63 ? 63 : app->pattern_len);
                        FURI_LOG_I(TAG, "Cyclic(%u): %s", app->pattern_len, app->cyclic_buf);
                    }
                    break;
                case InputKeyUp:
                    if(app->mode == PwnModeCyclic) {
                        app->pattern_len += 16;
                        if(app->pattern_len > 1024) app->pattern_len = 1024;
                    } else {
                        if(app->scroll_pos > 0) app->scroll_pos--;
                    }
                    break;
                case InputKeyDown:
                    if(app->mode == PwnModeCyclic) {
                        if(app->pattern_len > 16) app->pattern_len -= 16;
                    } else {
                        app->scroll_pos++;
                    }
                    break;
                case InputKeyRight:
                    app->mode = (app->mode + 1) % PwnModeCount;
                    app->scroll_pos = 0;
                    break;
                case InputKeyLeft:
                    app->mode = app->mode == 0 ? PwnModeCount - 1 : app->mode - 1;
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
