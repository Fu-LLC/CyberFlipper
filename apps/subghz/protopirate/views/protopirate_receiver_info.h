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
// views/protopirate_receiver_info.h
#pragma once

#include <gui/view.h>
#include "../helpers/protopirate_types.h"

typedef struct ProtoPirateReceiverInfo ProtoPirateReceiverInfo;

typedef void (*ProtoPirateReceiverInfoCallback)(ProtoPirateCustomEvent event, void* context);

void protopirate_view_receiver_info_set_callback(
    ProtoPirateReceiverInfo* receiver_info,
    ProtoPirateReceiverInfoCallback callback,
    void* context);

ProtoPirateReceiverInfo* protopirate_view_receiver_info_alloc(void);
void protopirate_view_receiver_info_free(ProtoPirateReceiverInfo* receiver_info);
View* protopirate_view_receiver_info_get_view(ProtoPirateReceiverInfo* receiver_info);

void protopirate_view_receiver_info_set_protocol_name(
    ProtoPirateReceiverInfo* receiver_info,
    const char* protocol_name);

void protopirate_view_receiver_info_set_info_text(
    ProtoPirateReceiverInfo* receiver_info,
    const char* info_text);

