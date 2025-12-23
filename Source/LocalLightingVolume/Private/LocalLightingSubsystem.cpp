// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

// Header Include
#include "LocalLightingSubsystem.h"

// Engine Include
#include "Subsystems/SubsystemBlueprintLibrary.h"

ULocalLightingSubsystem::ULocalLightingSubsystem()
{
}

bool ULocalLightingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> DerivedClasses;
	GetDerivedClasses(StaticClass(), DerivedClasses);
	return DerivedClasses.Num() == 0;
}

void ULocalLightingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Volumes.Reset();
}

ULocalLightingSubsystem* ULocalLightingSubsystem::Get(UObject* WorldContextObject)
{
	ULocalLightingSubsystem* Subsystem = Cast<ULocalLightingSubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(WorldContextObject, StaticClass()));
	return Subsystem;
}

void ULocalLightingSubsystem::ProcessVolume(const FVector& ViewPoint)
{
	Volumes.StableSort([](const TScriptInterface<IInterface_LocalLightingVolume>& A, const TScriptInterface<IInterface_LocalLightingVolume>& B)
	{
		return A->IsOverridingLighting() && !B->IsOverridingLighting();
	});
	for (TScriptInterface<IInterface_LocalLightingVolume>& Volume : Volumes)
	{
		if (Volume.GetInterface())
		{
			Volume->Process(ViewPoint);
		}
	}
}

void ULocalLightingSubsystem::RegisterVolume(IInterface_LocalLightingVolume* Volume)
{
	if (Volume)
	{
		TScriptInterface<IInterface_LocalLightingVolume> Interface;
		Interface.SetObject(Cast<UObject>(Volume));
		Interface.SetInterface(Volume);
		Volumes.AddUnique(Interface);
	}
}

void ULocalLightingSubsystem::UnregisterVolume(IInterface_LocalLightingVolume* Volume)
{
	if (Volume)
	{
		for (auto It = Volumes.CreateIterator(); It; ++It)
		{
			if (*It == Volume)
			{
				It.RemoveCurrent();
			}
		}
	}
}
