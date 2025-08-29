/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
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