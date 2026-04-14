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
// protopirate_history.h
#pragma once

#include <lib/subghz/receiver.h>
#include <lib/subghz/protocols/base.h>

#define PROTOPIRATE_HISTORY_MAX 20

typedef struct ProtoPirateHistory ProtoPirateHistory;

ProtoPirateHistory* protopirate_history_alloc(void);
void protopirate_history_free(ProtoPirateHistory* instance);
void protopirate_history_reset(ProtoPirateHistory* instance);
uint16_t protopirate_history_get_item(ProtoPirateHistory* instance);
uint16_t protopirate_history_get_last_index(ProtoPirateHistory* instance);
bool protopirate_history_add_to_history(
    ProtoPirateHistory* instance,
    void* context,
    SubGhzRadioPreset* preset);
void protopirate_history_get_text_item_menu(
    ProtoPirateHistory* instance,
    FuriString* output,
    uint16_t idx);
void protopirate_history_get_text_item(
    ProtoPirateHistory* instance,
    FuriString* output,
    uint16_t idx);
SubGhzProtocolDecoderBase*
    protopirate_history_get_decoder_base(ProtoPirateHistory* instance, uint16_t idx);
FlipperFormat* protopirate_history_get_raw_data(ProtoPirateHistory* instance, uint16_t idx);

void protopirate_history_set_item_str(
    ProtoPirateHistory* instance,
    uint16_t idx,
    const char* str);

