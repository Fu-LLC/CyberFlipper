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

#include <stdint.h>

#include <lib/subghz/environment.h>
#include <lib/subghz/subghz_keystore.h>

extern uint64_t kia_mf_key;
extern uint64_t kia_v6_a_key;
extern uint64_t kia_v6_b_key;
extern uint64_t kia_v5_key;

uint64_t get_kia_mf_key();

uint64_t get_kia_v6_keystore_a();

uint64_t get_kia_v6_keystore_b();

uint64_t get_kia_v5_key();

void protopirate_keys_load(SubGhzEnvironment* environment);

