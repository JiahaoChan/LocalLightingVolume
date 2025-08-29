/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
 */

#include "LocalLightingVolumeView.h"

#include "SceneView.h"
#include "SceneInterface.h"

#include "LocalLightingSubsystem.h"

FLocalLightingVolumeViewExtension::FLocalLightingVolumeViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
	
}

void FLocalLightingVolumeViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	if (UWorld* World = InViewFamily.Scene->GetWorld())
	{
		if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(World))
		{
			Subsystem->ProcessVolume(InView.ViewLocation);
		}
	}
}