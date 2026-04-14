/******************************************************************************
 *   ______  __   __  ______  ______  ______  ______  __      __  ______  ______  
 *  /\  ___\/\ \__\ \/\  == \/\  ___\/\  == \/\  ___\/\ \    /\ \/\  ___\/\  == \ 
 *  \ \ \___\ \____  \ \  __< \ \  __\\ \  __< \ \  __\\ \ \__\ \ \ \  __\\ \  __< 
 *   \ \_____\/\_____\ \_____\ \_____\ \_\ \_\ \_\     \ \_____\ \_\ \_____\ \_\ \_\
 *    \/_____/\/_____/\/_____/\/_____/\/_/ /_/\/_/      \/_____/\/_/\/_____/\/_/ /_/
 *                                                                                 
 *  PROJECT: CYBERFLIPPER v1.0
 *  DEVELOPER: Furulie LLC (FLLC)
 *  LICENSE: GNU General Public License v3.0
 *  DESCRIPTION: Authorized Security Research & Hardware Analysis Toolset
 ******************************************************************************/
#pragma once

#include <gui/gui.h>
#include <furi.h>
#include <furi_hal.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <stdint.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    uint32_t frequency;
    uint8_t cursor_position;
    bool running;
    const SubGhzDevice* device;
    SubGhzTxRxWorker* subghz_txrx;
    FuriThread* tx_thread;
    bool tx_running;
    int jamming_mode;
} JammerApp;

typedef enum {
    JammerModeOok650Async,
    JammerMode2FSKDev238Async,
    JammerMode2FSKDev476Async,
    JammerModeMSK99_97KbAsync,
    JammerModeGFSK9_99KbAsync,
    JammerModeBruteforce,
    JammerModeSineWave,
    JammerModeSquareWave,
    JammerModeSawtoothWave,
    JammerModeWhiteNoise,
    JammerModeTriangleWave,
    JammerModeChirp,
    JammerModeGaussianNoise,
    JammerModeBurst,
} JammerMode;

JammerApp* jammer_app_alloc(void);
void jammer_app_free(JammerApp* app);
int32_t jammer_app(void* p);

