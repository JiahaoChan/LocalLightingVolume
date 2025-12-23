// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

#pragma once

// Engine Include
#include "CoreMinimal.h"
#include "UObject/Interface.h"

// Generated Include
#include "Interface_LocalLightingVolume.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInterface_LocalLightingVolume : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInterface_LocalLightingVolume
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void Process(const FVector& ViewPoint) = 0;
	virtual bool IsOverridingLighting() const = 0;
};

UCLASS(Abstract, HideCategories = (Advanced, Collision, Volume, Brush, Attachment), MinimalAPI)
class ALocalLightingVolumeBase : public AVolume, public IInterface_LocalLightingVolume
{
	GENERATED_BODY()

protected:
	bool bViewPointInVolume;
	bool bOverridingLighting;

public:
	ALocalLightingVolumeBase();

	//~ Begin AActor Interface
	virtual void PostRegisterAllComponents() override;
	virtual void PostUnregisterAllComponents() override;
	//~ End AActor Interface

	//~ Begin IInterface_LocalLightingVolume Interface
	virtual void Process(const FVector& ViewPoint) override;
	virtual bool IsOverridingLighting() const override;
	//~ End IInterface_LocalLightingVolume Interface

protected:
	virtual void OverrideLighting() {}
	virtual void RestoreLighting() {}

public:
#if WITH_EDITOR
	FDelegateHandle PreSaveHandle;
	FDelegateHandle SavedHandle;
	virtual void OnOverridingLightComponentPackagePreSave(UPackage* Package, FObjectPreSaveContext Context);
	virtual void OnOverridingLightComponentPackageSaved(const FString& FileName, UPackage* Package, FObjectPostSaveContext Context);
#endif

private:
	void RegisterIntoSubsystem();
	void UnregisterFromSubsystem();
};
