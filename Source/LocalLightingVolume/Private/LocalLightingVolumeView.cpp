/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
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
	if (InViewFamily.Scene)
	{
		if (UWorld* World = InViewFamily.Scene->GetWorld())
        {
        	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(World))
        	{
        		Subsystem->ProcessVolume(InView.ViewLocation);
        	}
        }
	}
}