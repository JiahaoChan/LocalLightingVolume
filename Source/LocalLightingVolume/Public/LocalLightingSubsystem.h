// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

#pragma once

// Engine Include
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

// Plugins Include
#include "Interface_LocalLightingVolume.h"

// Generated Include
#include "LocalLightingSubsystem.generated.h"

UCLASS(NotBlueprintable)
class LOCALLIGHTINGVOLUME_API ULocalLightingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<TScriptInterface<IInterface_LocalLightingVolume>> Volumes;

public:
	ULocalLightingSubsystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static ULocalLightingSubsystem* Get(UObject* WorldContextObject);

	void ProcessVolume(const FVector& ViewPoint);

	void RegisterVolume(IInterface_LocalLightingVolume* Volume);

	void UnregisterVolume(IInterface_LocalLightingVolume* Volume);
};
