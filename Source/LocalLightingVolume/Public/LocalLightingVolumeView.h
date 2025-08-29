/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

class FLocalLightingVolumeViewExtension : public FSceneViewExtensionBase
{
public:
	FLocalLightingVolumeViewExtension(const FAutoRegister& AutoRegister);
	
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	
private:
	inline static TSharedPtr<FLocalLightingVolumeViewExtension, ESPMode::ThreadSafe> Instance;
	
	struct FStaticConstructor
	{
		FStaticConstructor()
		{
			FCoreDelegates::OnPostEngineInit.AddLambda([]()
			{ 
				Instance = FSceneViewExtensions::NewExtension<FLocalLightingVolumeViewExtension>();
			});
			FCoreDelegates::OnEnginePreExit.AddLambda([]()
			{ 
				Instance = nullptr;
			});
		}
	};
	
	static inline FStaticConstructor StaticConstructor;
};