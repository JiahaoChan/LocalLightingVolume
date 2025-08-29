/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "Interface_LocalLightingVolume.h"

#include "LocalLightingSubsystem.generated.h"

UCLASS(NotBlueprintable)
class LOCALLIGHTINGVOLUME_API ULocalLightingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	TArray<IInterface_LocalLightingVolume*> Volumes;
	
public:
	ULocalLightingSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static ULocalLightingSubsystem* Get(UObject* WorldContextObject);
	
	void ProcessVolume(const FVector& ViewPoint);
	
	void RegisterVolume(IInterface_LocalLightingVolume* Volume);
	
	void UnregisterVolume(IInterface_LocalLightingVolume* Volume);
};