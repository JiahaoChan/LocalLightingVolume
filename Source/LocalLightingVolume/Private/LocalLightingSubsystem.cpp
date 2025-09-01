/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#include "LocalLightingSubsystem.h"

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
	for (IInterface_LocalLightingVolume* Volume : Volumes)
	{
		if (Volume)
		{
			Volume->Process(ViewPoint);
		}
	}
}

void ULocalLightingSubsystem::RegisterVolume(IInterface_LocalLightingVolume* Volume)
{
	if (Volume)
	{
		Volumes.AddUnique(Volume);
	}
}

void ULocalLightingSubsystem::UnregisterVolume(IInterface_LocalLightingVolume* Volume)
{
	if (Volume)
	{
		Volumes.Remove(Volume);
	}
}