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
#include <stdbool.h>

#define GARAGE_BF_TAG "GarageBruteforce"
#define GARAGE_MAX_CODE_BITS 12
#define GARAGE_MIN_CODE_BITS 8
#define DEBRUIJN_MAX_LEN 8192

typedef enum {
    GarageBFProtocolLinear,
    GarageBFProtocolNiceFlo,
    GarageBFProtocolCame,
    GarageBFProtocolChamberlain,
    GarageBFProtocolLiftmaster,
    GarageBFProtocolGenie,
    GarageBFProtocolDoorman,
    GarageBFProtocolGatehouse,
    GarageBFProtocolSMC5326,
    GarageBFProtocolHoltek,
    GarageBFProtocolCount,
} GarageBFProtocol;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    const SubGhzDevice* device;
    SubGhzTxRxWorker* subghz_txrx;
    FuriThread* tx_thread;
    
    uint32_t frequency;
    uint8_t code_bits;
    GarageBFProtocol protocol;
    bool running;
    bool tx_running;
    bool attack_active;
    
    uint32_t total_codes;
    uint32_t codes_sent;
    float progress;
    
    // De Bruijn sequence buffer
    uint8_t debruijn_seq[DEBRUIJN_MAX_LEN];
    uint32_t debruijn_len;
} GarageBFApp;

static const char* garage_protocol_names[] = {
    "Linear (10-bit)",
    "Nice FLO (12-bit)",
    "CAME (12-bit)",
    "Chamberlain (9-bit)",
    "LiftMaster (9-bit)",
    "Genie (12-bit)",
    "DoorMan (10-bit)",
    "GateHouse (10-bit)",
    "SMC5326 (8-bit)",
    "Holtek (12-bit)",
};

static const uint32_t garage_protocol_frequencies[] = {
    300000000,  // Linear
    433920000,  // Nice FLO
    433920000,  // CAME
    315000000,  // Chamberlain
    315000000,  // LiftMaster
    390000000,  // Genie
    433920000,  // DoorMan
    433920000,  // GateHouse
    330000000,  // SMC5326
    433920000,  // Holtek
};

static const uint8_t garage_protocol_bits[] = {
    10,  // Linear
    12,  // Nice FLO
    12,  // CAME
    9,   // Chamberlain
    9,   // LiftMaster
    12,  // Genie
    10,  // DoorMan
    10,  // GateHouse
    8,   // SMC5326
    12,  // Holtek
};

GarageBFApp* garage_bf_app_alloc(void);
void garage_bf_app_free(GarageBFApp* app);
int32_t garage_bruteforce_app(void* p);

