/**
 * Plugin LocalLightingVolume
 *      Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#include "LocalLightingVolume.h"

#define LOCTEXT_NAMESPACE "FLocalLightingVolumeModule"

void FLocalLightingVolumeModule::StartupModule()
{

}

void FLocalLightingVolumeModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLocalLightingVolumeModule, LocalLightingVolume)