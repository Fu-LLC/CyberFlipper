// CyberFlipper Custom .fap App Template
// Add your custom animation, branding, and UI logic here
// See Flipper Zero SDK documentation for details

#include <furi.h>
#include <gui/gui.h>

void cyberflipper_theme_app(void* p) {
    // Example: Draw a custom logo or animation
    // Use Flipper's graphics API for on-device visuals
}

int32_t cyberflipper_theme_app_main(void* p) {
    cyberflipper_theme_app(p);
    return 0;
}

// Register the app entry point
// APP_MAIN(cyberflipper_theme_app_main)
