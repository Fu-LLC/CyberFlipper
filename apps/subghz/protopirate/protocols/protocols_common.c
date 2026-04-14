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
#include "protocols_common.h"

const char* protopirate_get_short_preset_name(const char* preset_name) {
    if(!preset_name) {
        return "UNKNOWN";
    }
    if(!strcmp(preset_name, "FuriHalSubGhzPresetOok270Async")) {
        return "AM270";
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetOok650Async")) {
        return "AM650";
    } else if(!strcmp(preset_name, "FuriHalSubGhzPreset2FSKDev238Async")) {
        return "FM238";
    } else if(!strcmp(preset_name, "FuriHalSubGhzPreset2FSKDev12KAsync")) {
        return "FM12K";
    } else if(!strcmp(preset_name, "FuriHalSubGhzPreset2FSKDev476Async")) {
        return "FM476";
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetCustom")) {
        return "CUSTOM";
    }
    return preset_name;
}

