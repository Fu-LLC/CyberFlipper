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

#include <furi.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/types.h>
#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/generic.h>
#include <lib/subghz/blocks/math.h>
#include <flipper_format/flipper_format.h>

#include "../defines.h"

#define MITSUBISHI_PROTOCOL_NAME "Mitsubishi V0"

typedef struct SubGhzProtocolDecoderMitsubishi SubGhzProtocolDecoderMitsubishi;

extern const SubGhzProtocol mitsubishi_v0_protocol;

void* subghz_protocol_decoder_mitsubishi_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_mitsubishi_free(void* context);
void subghz_protocol_decoder_mitsubishi_reset(void* context);
void subghz_protocol_decoder_mitsubishi_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_mitsubishi_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_mitsubishi_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_mitsubishi_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_mitsubishi_get_string(void* context, FuriString* output);

