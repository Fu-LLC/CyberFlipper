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
// helpers/protopirate_types.h
#pragma once

#include <furi.h>
#include <furi_hal.h>

typedef enum {
    ProtoPirateViewVariableItemList,
    ProtoPirateViewSubmenu,
    ProtoPirateViewWidget,
    ProtoPirateViewReceiver,
    ProtoPirateViewReceiverInfo,
    ProtoPirateViewAbout,
    ProtoPirateViewFileBrowser,
    ProtoPirateViewTextInput,
} ProtoPirateView;

typedef enum {
    // Custom events for views
    ProtoPirateCustomEventViewReceiverOK,
    ProtoPirateCustomEventViewReceiverConfig,
    ProtoPirateCustomEventViewReceiverBack,
    ProtoPirateCustomEventViewReceiverUnlock,
    // Custom events for scenes
    ProtoPirateCustomEventSceneReceiverUpdate,
    ProtoPirateCustomEventSceneSettingLock,
    // File management
    ProtoPirateCustomEventReceiverInfoSave,
    ProtoPirateCustomEventReceiverInfoSaveConfirm,
    ProtoPirateCustomEventReceiverInfoEmulate,
    ProtoPirateCustomEventReceiverInfoBruteforceStart,
    ProtoPirateCustomEventReceiverInfoBruteforceCancel,
    ProtoPirateCustomEventSavedInfoDelete,
    // Emulator
    ProtoPirateCustomEventSavedInfoEmulate,
    ProtoPirateCustomEventEmulateTransmit,
    ProtoPirateCustomEventEmulateStop,
    ProtoPirateCustomEventEmulateExit,
    // Sub decode
    ProtoPirateCustomEventSubDecodeUpdate,
    ProtoPirateCustomEventSubDecodeSave,
    ProtoPirateCustomEventSubDecodeBruteforceStart,
    ProtoPirateCustomEventPsaBruteforceComplete,
    // File Browser
    ProtoPirateCustomEventSavedFileSelected,
    // Need saving confirmation
    ProtoPirateCustomEventSceneStay,
    ProtoPirateCustomEventSceneExit,
} ProtoPirateCustomEvent;

typedef enum {
    ProtoPirateLockOff,
    ProtoPirateLockOn,
} ProtoPirateLock;

typedef enum {
    ProtoPirateTxRxStateIDLE,
    ProtoPirateTxRxStateRx,
    ProtoPirateTxRxStateTx,
    ProtoPirateTxRxStateSleep,
} ProtoPirateTxRxState;

typedef enum {
    ProtoPirateHopperStateOFF,
    ProtoPirateHopperStateRunning,
    ProtoPirateHopperStatePause,
    ProtoPirateHopperStateRSSITimeOut,
} ProtoPirateHopperState;

typedef enum {
    ProtoPirateRxKeyStateIDLE,
    ProtoPirateRxKeyStateBack,
    ProtoPirateRxKeyStateStart,
    ProtoPirateRxKeyStateAddKey,
} ProtoPirateRxKeyState;
