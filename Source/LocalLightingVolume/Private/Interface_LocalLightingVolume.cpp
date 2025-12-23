// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

// Header Include
#include "Interface_LocalLightingVolume.h"

// Engine Include
#include "UObject/ObjectSaveContext.h"

// Plugins Include
#include "LocalLightingSubsystem.h"

UInterface_LocalLightingVolume::UInterface_LocalLightingVolume( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{

}

ALocalLightingVolumeBase::ALocalLightingVolumeBase()
{
	bViewPointInVolume = false;
	bOverridingLighting = false;
}

void ALocalLightingVolumeBase::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

#if WITH_EDITOR
	PreSaveHandle = UPackage::PreSavePackageWithContextEvent.AddUObject(this, &ALocalLightingVolumeBase::OnOverridingLightComponentPackagePreSave);
	SavedHandle = UPackage::PackageSavedWithContextEvent.AddUObject(this, &ALocalLightingVolumeBase::OnOverridingLightComponentPackageSaved);
#endif
	RegisterIntoSubsystem();
}

void ALocalLightingVolumeBase::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();

#if WITH_EDITOR
	UPackage::PreSavePackageWithContextEvent.Remove(PreSaveHandle);
	UPackage::PackageSavedWithContextEvent.Remove(SavedHandle);
#endif
	UnregisterFromSubsystem();
}

void ALocalLightingVolumeBase::Process(const FVector& ViewPoint)
{
	bool bViewPointInVolumeLastTime = bViewPointInVolume;
	bViewPointInVolume = EncompassesPoint(ViewPoint);
	if (bViewPointInVolumeLastTime != bViewPointInVolume)
	{
		if (bViewPointInVolume)
		{
			OverrideLighting();
		}
		else
		{
			RestoreLighting();
		}
	}
}

bool ALocalLightingVolumeBase::IsOverridingLighting() const
{
	return bOverridingLighting;
}

#if WITH_EDITOR
void ALocalLightingVolumeBase::OnOverridingLightComponentPackagePreSave(UPackage* Package, FObjectPreSaveContext Context)
{
	// We can assert that this Volume and other light components are all in the same UWorld package.
	if (Package == GetOutermost())
	{
		if (IsOverridingLighting())
		{
			// We can not be overriding any light component when they are saved.
			RestoreLighting();
		}
	}
}

void ALocalLightingVolumeBase::OnOverridingLightComponentPackageSaved(const FString& FileName, UPackage* Package, FObjectPostSaveContext Context)
{
	// We can assert that this Volume and other light components are all in the same UWorld package.
	if (Package == GetOutermost())
	{
		if (bViewPointInVolume)
		{
			OverrideLighting();
		}
	}
}
#endif

void ALocalLightingVolumeBase::RegisterIntoSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->RegisterVolume(this);
	}
}

void ALocalLightingVolumeBase::UnregisterFromSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->UnregisterVolume(this);
	}
}
