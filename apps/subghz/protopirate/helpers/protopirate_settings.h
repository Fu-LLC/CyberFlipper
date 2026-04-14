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
// helpers/protopirate_settings.h
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PROTOPIRATE_SETTINGS_FILE APP_DATA_PATH("settings.txt")
#define PROTOPIRATE_SETTINGS_DIR  APP_DATA_PATH()

typedef struct {
    uint32_t frequency;
    uint8_t preset_index;
    uint8_t tx_power;
    bool auto_save;
    bool hopping_enabled;
} ProtoPirateSettings;

void protopirate_settings_load(ProtoPirateSettings* settings);
void protopirate_settings_save(ProtoPirateSettings* settings);
void protopirate_settings_set_defaults(ProtoPirateSettings* settings);

