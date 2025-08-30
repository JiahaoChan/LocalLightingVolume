/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#include "LocalDirectionalLightVolume.h"

#include "Components/BrushComponent.h"
#include "Components/LightComponent.h"
#include "Engine/DirectionalLight.h"

#include "LocalLightingSubsystem.h"

ALocalDirectionalLightVolume::ALocalDirectionalLightVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->bAlwaysCreatePhysicsState = true;
	GetBrushComponent()->Mobility = EComponentMobility::Movable;
	
	bOverride_Rotation = false;
	bOverride_Intensity = false;
	bOverride_LightColor = false;
	bOverride_IndirectLightingIntensity = false;
	bOverride_VolumetricScatteringIntensity = false;
	
	DirectionalLight = nullptr;
	Rotation = FRotator(-46.0f, 0.0f, 0.0f);
	Intensity = 1.0f;
	LightColor = FColor::White;
	IndirectLightingIntensity = 1.0f;
	VolumetricScatteringIntensity = 1.0f;
	
	bViewPointInVolume = false;
	bOverridingLighting = false;
	
	CacheDirectionalLight = nullptr;
	CacheRotation = FRotator(-46.0f, 0.0f, 0.0f);
	CacheIntensity = 1.0f;
	CacheLightColor = FColor::White;
	CacheIndirectLightingIntensity = 1.0f;
	CacheVolumetricScatteringIntensity = 1.0f;
	
#if WITH_EDITORONLY_DATA
	bCacheOverride_Rotation = false;
	bCacheOverride_Intensity = false;
	bCacheOverride_LightColor = false;
	bCacheOverride_IndirectLightingIntensity = false;
	bCacheOverride_VolumetricScatteringIntensity = false;
#endif
}

void ALocalDirectionalLightVolume::Process(const FVector& ViewPoint)
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

bool ALocalDirectionalLightVolume::IsOverridingLighting()
{
	return bOverridingLighting;
}

void ALocalDirectionalLightVolume::RegisterIntoSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->RegisterVolume(this);
	}
}

void ALocalDirectionalLightVolume::UnregisterFromSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->UnregisterVolume(this);
	}
}

void ALocalDirectionalLightVolume::OverrideLighting()
{
	bOverridingLighting = false;
	if (DirectionalLight.IsValid())
	{
		if (bOverride_Rotation)
		{
			CacheRotation = DirectionalLight->GetLightComponent()->GetComponentTransform().Rotator();
			if (CacheRotation != Rotation)
			{
				DirectionalLight->GetLightComponent()->SetWorldRotation(Rotation);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_Intensity)
		{
			CacheIntensity = DirectionalLight->GetLightComponent()->Intensity;
			if (CacheIntensity != Intensity)
			{
				DirectionalLight->GetLightComponent()->SetIntensity(Intensity);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_LightColor)
		{
			CacheLightColor = DirectionalLight->GetLightComponent()->GetLightColor().ToFColor(true);
			if (CacheLightColor != LightColor)
			{
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(LightColor));
				bOverridingLighting |= true;
			}
		}
		if (bOverride_IndirectLightingIntensity)
		{
			CacheIndirectLightingIntensity = DirectionalLight->GetLightComponent()->IndirectLightingIntensity;
			if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
			{
				DirectionalLight->GetLightComponent()->SetIndirectLightingIntensity(IndirectLightingIntensity);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_VolumetricScatteringIntensity)
		{
			CacheVolumetricScatteringIntensity = DirectionalLight->GetLightComponent()->VolumetricScatteringIntensity;
			if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
			{
				DirectionalLight->GetLightComponent()->SetVolumetricScatteringIntensity(VolumetricScatteringIntensity);
				bOverridingLighting |= true;
			}
		}
	}
}

void ALocalDirectionalLightVolume::RestoreLighting()
{
	if (DirectionalLight.IsValid())
	{
		if (bOverride_Rotation)
		{
			if (CacheRotation != Rotation)
			{
				DirectionalLight->GetLightComponent()->SetWorldRotation(CacheRotation);
			}
		}
		if (bOverride_Intensity)
		{
			if (CacheIntensity != Intensity)
			{
				DirectionalLight->GetLightComponent()->SetIntensity(CacheIntensity);
			}
		}
		if (bOverride_LightColor)
		{
			if (CacheLightColor != LightColor)
			{
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
			}
		}
		if (bOverride_IndirectLightingIntensity)
		{
			if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
			{
				DirectionalLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
			}
		}
		if (bOverride_VolumetricScatteringIntensity)
		{
			if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
			{
				DirectionalLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
			}
		}
	}
	bOverridingLighting = false;
}

void ALocalDirectionalLightVolume::PostRegisterAllComponents()
{
 	Super::PostRegisterAllComponents();
	
	RegisterIntoSubsystem();
}

void ALocalDirectionalLightVolume::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();
	
	UnregisterFromSubsystem();
}

#if WITH_EDITOR
void ALocalDirectionalLightVolume::PreEditChange(FProperty* PropertyAboutToChange)
{
	if (PropertyAboutToChange)
	{
		const FString PropertyName = PropertyAboutToChange->GetName();
		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, DirectionalLight))
		{
			CacheDirectionalLight = DirectionalLight;
		}
		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, Rotation) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, Intensity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, LightColor) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, IndirectLightingIntensity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalDirectionalLightVolume, VolumetricScatteringIntensity))
		{
			bCacheOverride_Rotation = bOverride_Rotation;
			bCacheOverride_Intensity = bOverride_Intensity;
			bCacheOverride_LightColor = bOverride_LightColor;
			bCacheOverride_IndirectLightingIntensity = bOverride_IndirectLightingIntensity;
			bCacheOverride_VolumetricScatteringIntensity = bOverride_VolumetricScatteringIntensity;
		}
	}
	Super::PreEditChange(PropertyAboutToChange);
}

void ALocalDirectionalLightVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, DirectionalLight))
	{
		if (bViewPointInVolume && CacheDirectionalLight != DirectionalLight)
		{
			// Restore lighting to previous Directional Light.
			if (CacheDirectionalLight.IsValid())
			{
				if (bOverride_Rotation)
				{
					if (CacheRotation != Rotation)
					{
						CacheDirectionalLight->GetLightComponent()->SetWorldRotation(CacheRotation);
					}
				}
				if (bOverride_Intensity)
				{
					if (CacheIntensity != Intensity)
					{
						CacheDirectionalLight->GetLightComponent()->SetIntensity(CacheIntensity);
					}
				}
				if (bOverride_LightColor)
				{
					if (CacheLightColor != LightColor)
					{
						CacheDirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
					}
				}
				if (bOverride_IndirectLightingIntensity)
				{
					if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
					{
						CacheDirectionalLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
					}
				}
				if (bOverride_VolumetricScatteringIntensity)
				{
					if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
					{
						CacheDirectionalLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
					}
				}
			}
			OverrideLighting();
		}
	}
	
	if (bViewPointInVolume)
	{
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, Rotation))
		{
			if (DirectionalLight.IsValid())
			{
				if (bOverride_Rotation)
				{
					if (bOverride_Rotation != bCacheOverride_Rotation)
					{
						CacheRotation = DirectionalLight->GetLightComponent()->GetComponentTransform().Rotator();
					}
					DirectionalLight->GetLightComponent()->SetWorldRotation(Rotation);
				}
				else
				{
					DirectionalLight->GetLightComponent()->SetWorldRotation(CacheRotation);
				}
			}
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, Intensity))
		{
			if (DirectionalLight.IsValid())
			{
				if (bOverride_Intensity)
				{
					if (bOverride_Intensity != bCacheOverride_Intensity)
					{
						CacheIntensity = DirectionalLight->GetLightComponent()->Intensity;
					}
					DirectionalLight->GetLightComponent()->SetIntensity(Intensity);
				}
				else
				{
					DirectionalLight->GetLightComponent()->SetIntensity(CacheIntensity);
				}
			}
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, LightColor))
		{
			if (DirectionalLight.IsValid())
			{
				if (bOverride_LightColor)
				{
					if (bOverride_LightColor != bCacheOverride_LightColor)
					{
						CacheLightColor = DirectionalLight->GetLightComponent()->GetLightColor().ToFColor(true);
					}
					DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(LightColor));
				}
				else
				{
					DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
				}
			}
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, IndirectLightingIntensity))
		{
			if (DirectionalLight.IsValid())
			{
				if (bOverride_IndirectLightingIntensity)
				{
					if (bOverride_IndirectLightingIntensity != bCacheOverride_IndirectLightingIntensity)
					{
						CacheIndirectLightingIntensity = DirectionalLight->GetLightComponent()->IndirectLightingIntensity;
					}
					DirectionalLight->GetLightComponent()->SetIndirectLightingIntensity(IndirectLightingIntensity);
				}
				else
				{
					DirectionalLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
				}
			}
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, VolumetricScatteringIntensity))
		{
			if (DirectionalLight.IsValid())
			{
				if (bOverride_VolumetricScatteringIntensity)
				{
					if (bOverride_VolumetricScatteringIntensity != bCacheOverride_VolumetricScatteringIntensity)
					{
						CacheVolumetricScatteringIntensity = DirectionalLight->GetLightComponent()->VolumetricScatteringIntensity;
					}
					DirectionalLight->GetLightComponent()->SetVolumetricScatteringIntensity(VolumetricScatteringIntensity);
				}
				else
				{
					DirectionalLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
				}
			}
		}
	}
}

bool ALocalDirectionalLightVolume::CanEditChange(const FProperty* InProperty) const
{
	const FName PropertyName = InProperty->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, Rotation) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, Intensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, LightColor) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, IndirectLightingIntensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalDirectionalLightVolume, VolumetricScatteringIntensity))
	{
		return DirectionalLight.IsValid();
	}
	
	return Super::CanEditChange(InProperty);
}
#endif