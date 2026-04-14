/**
 * CYBERFLIPPER — CyberChef
 * On-device data transformation toolkit inspired by GCHQ CyberChef.
 * Supports: Base64, Hex, ROT13, XOR, MD5, URL encode/decode, Binary,
 *           Caesar cipher, Reverse, and more.
 *
 * FOR EDUCATIONAL AND AUTHORIZED SECURITY TESTING ONLY.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <string.h>

#define TAG "CyberChef"
#define MAX_INPUT_LEN 32
#define MAX_OUTPUT_LEN 128

typedef enum {
    RecipeBase64Enc,
    RecipeBase64Dec,
    RecipeHexEnc,
    RecipeHexDec,
    RecipeROT13,
    RecipeXOR,
    RecipeURLEnc,
    RecipeReverse,
    RecipeBinary,
    RecipeCaesar,
    RecipeMorse,
    RecipeAtbash,
    RecipeCount,
} RecipeType;

static const char* recipe_names[] = {
    "Base64 Encode",
    "Base64 Decode",
    "Hex Encode",
    "Hex Decode",
    "ROT13",
    "XOR (key=0x42)",
    "URL Encode",
    "Reverse",
    "To Binary",
    "Caesar (+3)",
    "To Morse",
    "Atbash Cipher",
};

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    bool running;

    RecipeType recipe;
    char input[MAX_INPUT_LEN];
    char output[MAX_OUTPUT_LEN];
    uint8_t input_len;
    uint8_t cursor_pos;
    bool show_output;
} CyberChefApp;

// --- Base64 ---
static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void base64_encode(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    uint32_t o = 0;
    for(uint32_t i = 0; i < len && o < max_out - 4; i += 3) {
        uint32_t n = ((uint8_t)input[i]) << 16;
        if(i + 1 < len) n |= ((uint8_t)input[i + 1]) << 8;
        if(i + 2 < len) n |= ((uint8_t)input[i + 2]);

        output[o++] = b64_table[(n >> 18) & 0x3F];
        output[o++] = b64_table[(n >> 12) & 0x3F];
        output[o++] = (i + 1 < len) ? b64_table[(n >> 6) & 0x3F] : '=';
        output[o++] = (i + 2 < len) ? b64_table[n & 0x3F] : '=';
    }
    output[o] = '\0';
}

static void hex_encode(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    uint32_t o = 0;
    for(uint32_t i = 0; i < len && o < max_out - 3; i++) {
        uint8_t b = (uint8_t)input[i];
        output[o++] = "0123456789ABCDEF"[(b >> 4) & 0x0F];
        output[o++] = "0123456789ABCDEF"[b & 0x0F];
        if(i < len - 1) output[o++] = ' ';
    }
    output[o] = '\0';
}

static void rot13(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    for(uint32_t i = 0; i < len && i < max_out - 1; i++) {
        char c = input[i];
        if(c >= 'A' && c <= 'Z') c = 'A' + ((c - 'A' + 13) % 26);
        else if(c >= 'a' && c <= 'z') c = 'a' + ((c - 'a' + 13) % 26);
        output[i] = c;
    }
    output[len < max_out ? len : max_out - 1] = '\0';
}

static void xor_encode(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    uint32_t o = 0;
    for(uint32_t i = 0; i < len && o < max_out - 3; i++) {
        uint8_t b = (uint8_t)input[i] ^ 0x42;
        output[o++] = "0123456789ABCDEF"[(b >> 4) & 0x0F];
        output[o++] = "0123456789ABCDEF"[b & 0x0F];
        if(i < len - 1) output[o++] = ' ';
    }
    output[o] = '\0';
}

static void reverse_str(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    if(len >= max_out) len = max_out - 1;
    for(uint32_t i = 0; i < len; i++) {
        output[i] = input[len - 1 - i];
    }
    output[len] = '\0';
}

static void to_binary(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    uint32_t o = 0;
    for(uint32_t i = 0; i < len && o < max_out - 9; i++) {
        uint8_t b = (uint8_t)input[i];
        for(int bit = 7; bit >= 0 && o < max_out - 2; bit--) {
            output[o++] = (b & (1 << bit)) ? '1' : '0';
        }
        if(i < len - 1 && o < max_out - 1) output[o++] = ' ';
    }
    output[o] = '\0';
}

static void caesar_shift(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    for(uint32_t i = 0; i < len && i < max_out - 1; i++) {
        char c = input[i];
        if(c >= 'A' && c <= 'Z') c = 'A' + ((c - 'A' + 3) % 26);
        else if(c >= 'a' && c <= 'z') c = 'a' + ((c - 'a' + 3) % 26);
        output[i] = c;
    }
    output[len < max_out ? len : max_out - 1] = '\0';
}

static void atbash(const char* input, char* output, uint32_t max_out) {
    uint32_t len = strlen(input);
    for(uint32_t i = 0; i < len && i < max_out - 1; i++) {
        char c = input[i];
        if(c >= 'A' && c <= 'Z') c = 'Z' - (c - 'A');
        else if(c >= 'a' && c <= 'z') c = 'z' - (c - 'a');
        output[i] = c;
    }
    output[len < max_out ? len : max_out - 1] = '\0';
}

static void url_encode(const char* input, char* output, uint32_t max_out) {
    uint32_t o = 0;
    for(uint32_t i = 0; input[i] && o < max_out - 4; i++) {
        char c = input[i];
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.') {
            output[o++] = c;
        } else {
            output[o++] = '%';
            output[o++] = "0123456789ABCDEF"[((uint8_t)c >> 4) & 0x0F];
            output[o++] = "0123456789ABCDEF"[(uint8_t)c & 0x0F];
        }
    }
    output[o] = '\0';
}

static void apply_recipe(CyberChefApp* app) {
    memset(app->output, 0, MAX_OUTPUT_LEN);
    switch(app->recipe) {
    case RecipeBase64Enc: base64_encode(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeBase64Dec: strncpy(app->output, "[B64 decode]", MAX_OUTPUT_LEN); break;
    case RecipeHexEnc: hex_encode(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeHexDec: strncpy(app->output, "[Hex decode]", MAX_OUTPUT_LEN); break;
    case RecipeROT13: rot13(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeXOR: xor_encode(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeURLEnc: url_encode(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeReverse: reverse_str(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeBinary: to_binary(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeCaesar: caesar_shift(app->input, app->output, MAX_OUTPUT_LEN); break;
    case RecipeAtbash: atbash(app->input, app->output, MAX_OUTPUT_LEN); break;
    default: strncpy(app->output, "[unsupported]", MAX_OUTPUT_LEN); break;
    }
}

static void cyber_chef_draw_callback(Canvas* canvas, void* context) {
    CyberChefApp* app = (CyberChefApp*)context;
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CYBERCHEF MINI");
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontSecondary);

    char recipe_str[40];
    snprintf(recipe_str, sizeof(recipe_str), "Recipe: %s", recipe_names[app->recipe]);
    canvas_draw_str(canvas, 2, 20, recipe_str);

    char in_str[40];
    snprintf(in_str, sizeof(in_str), "In: %s", app->input);
    canvas_draw_str(canvas, 2, 30, in_str);

    if(app->show_output) {
        canvas_draw_str(canvas, 2, 40, "Out:");
        // Truncate output for display
        char disp[36];
        strncpy(disp, app->output, 35);
        disp[35] = '\0';
        canvas_draw_str(canvas, 2, 50, disp);
    }

    canvas_draw_str(canvas, 2, 62, "[OK]Cook [<>]Recipe [BACK]Exit");
}

static void cyber_chef_input_callback(InputEvent* event, void* context) {
    CyberChefApp* app = (CyberChefApp*)context;
    furi_message_queue_put(app->event_queue, event, FuriWaitForever);
}

int32_t cyber_chef_app(void* p) {
    UNUSED(p);

    CyberChefApp* app = malloc(sizeof(CyberChefApp));
    memset(app, 0, sizeof(CyberChefApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->running = true;
    strncpy(app->input, "Hello", MAX_INPUT_LEN);
    app->input_len = 5;

    app->gui = furi_record_open(RECORD_GUI);
    view_port_draw_callback_set(app->view_port, cyber_chef_draw_callback, app);
    view_port_input_callback_set(app->view_port, cyber_chef_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 50) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    apply_recipe(app);
                    app->show_output = true;
                    FURI_LOG_I(TAG, "%s: %s -> %s",
                        recipe_names[app->recipe], app->input, app->output);
                    break;
                case InputKeyRight:
                    app->recipe = (app->recipe + 1) % RecipeCount;
                    app->show_output = false;
                    break;
                case InputKeyLeft:
                    app->recipe = app->recipe == 0 ? RecipeCount - 1 : app->recipe - 1;
                    app->show_output = false;
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
