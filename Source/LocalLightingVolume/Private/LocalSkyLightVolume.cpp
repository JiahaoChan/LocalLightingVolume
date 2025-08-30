/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#include "LocalSkyLightVolume.h"

#include "Components/BrushComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/SkyLight.h"

#include "LocalLightingSubsystem.h"

/**
 * Had better to add this function into Engine
 *		void USkyLightComponent::SetLowerHemisphereIsBlack(bool InbLowerHemisphereIsBlack)
 */
void SetLowerHemisphereIsBlack(USkyLightComponent* SkyLightComponent, bool InbLowerHemisphereIsBlack)
{
	if (!SkyLightComponent)
	{
		return;
	}
	
	// Can't set on a static light
	auto AreDynamicDataChangesAllowed = [](const USkyLightComponent* Component, bool bIgnoreStationary = true)
	{
		if (!Component)
		{
			return false;
		}
		return (Component->IsOwnerRunningUserConstructionScript()) || !(Component->IsRegistered() && (Component->Mobility == EComponentMobility::Static || (!bIgnoreStationary && Component->Mobility == EComponentMobility::Stationary)));
	};
	
	if (AreDynamicDataChangesAllowed(SkyLightComponent) && SkyLightComponent->bLowerHemisphereIsBlack != InbLowerHemisphereIsBlack)
	{
		SkyLightComponent->bLowerHemisphereIsBlack = InbLowerHemisphereIsBlack;
		SkyLightComponent->MarkRenderStateDirty();
		SkyLightComponent->SetCaptureIsDirty();
	}
}

ALocalSkyLightVolume::ALocalSkyLightVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->bAlwaysCreatePhysicsState = true;
	GetBrushComponent()->Mobility = EComponentMobility::Movable;
	
	bOverride_bRealTimeCapture = false;
	bOverride_SourceType = false;
	bOverride_Cubemap = false;
	bOverride_Intensity = false;
	bOverride_LightColor = false;
	bOverride_bLowerHemisphereIsBlack = false;
	bOverride_LowerHemisphereColor = false;
	
	SkyLight = nullptr;
	bRealTimeCapture = false;
	SourceType = SLS_CapturedScene;
	Cubemap = nullptr;
	Intensity = 1.0f;
	LightColor = FColor::White;
	IndirectLightingIntensity = 1.0f;
	VolumetricScatteringIntensity = 1.0f;
	bLowerHemisphereIsBlack = true;
	LowerHemisphereColor = FLinearColor::Black;
	
	bViewPointInVolume = false;
	bOverridingLighting = false;
	
	CacheSkyLight = nullptr;
	bCacheRealTimeCapture = false;
	CacheSourceType = SLS_CapturedScene;
	CacheCubemap = nullptr;
	CacheIntensity = 1.0f;
	CacheLightColor = FColor::White;
	CacheIndirectLightingIntensity = 1.0f;
	CacheVolumetricScatteringIntensity = 1.0f;
	bCacheLowerHemisphereIsBlack = true;
	CacheLowerHemisphereColor = FLinearColor::Black;
	
#if WITH_EDITORONLY_DATA
	bCacheOverride_bRealTimeCapture = false;
	bCacheOverride_SourceType = false;
	bCacheOverride_Cubemap = false;
	bCacheOverride_Intensity = false;
	bCacheOverride_LightColor = false;
	bCacheOverride_IndirectLightingIntensity = false;
	bCacheOverride_VolumetricScatteringIntensity = false;
	bCacheOverride_bLowerHemisphereIsBlack = false;
	bCacheOverride_LowerHemisphereColor = false;
#endif
}

void ALocalSkyLightVolume::Process(const FVector& ViewPoint)
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

bool ALocalSkyLightVolume::IsOverridingLighting()
{
	return bOverridingLighting;
}

void ALocalSkyLightVolume::RegisterIntoSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->RegisterVolume(this);
	}
}

void ALocalSkyLightVolume::UnregisterFromSubsystem()
{
	if (ULocalLightingSubsystem* Subsystem = ULocalLightingSubsystem::Get(this))
	{
		Subsystem->UnregisterVolume(this);
	}
}

void ALocalSkyLightVolume::OverrideLighting()
{
	bOverridingLighting = false;
	if (SkyLight.IsValid())
	{
		if (bOverride_bRealTimeCapture)
		{
			bCacheRealTimeCapture = SkyLight->GetLightComponent()->bRealTimeCapture;
			if (bCacheRealTimeCapture != bRealTimeCapture)
			{
				SkyLight->GetLightComponent()->SetRealTimeCapture(bRealTimeCapture);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_SourceType)
		{
			CacheSourceType = SkyLight->GetLightComponent()->SourceType;
			if (CacheSourceType != SourceType)
			{
				SkyLight->GetLightComponent()->SourceType = SourceType;
				SkyLight->GetLightComponent()->MarkRenderStateDirty();
				SkyLight->GetLightComponent()->SetCaptureIsDirty();
				bOverridingLighting |= true;
			}
		}
		if (bOverride_Cubemap)
		{
			CacheCubemap = SkyLight->GetLightComponent()->Cubemap;
			if (CacheCubemap != Cubemap)
			{
				SkyLight->GetLightComponent()->SetCubemap(Cubemap);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_Intensity)
		{
			CacheIntensity = SkyLight->GetLightComponent()->Intensity;
			if (CacheIntensity != Intensity)
			{
				SkyLight->GetLightComponent()->SetIntensity(Intensity);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_LightColor)
		{
			CacheLightColor = SkyLight->GetLightComponent()->GetLightColor().ToFColor(true);
			if (CacheLightColor != LightColor)
			{
				SkyLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(LightColor));
				bOverridingLighting |= true;
			}
		}
		if (bOverride_IndirectLightingIntensity)
		{
			CacheIndirectLightingIntensity = SkyLight->GetLightComponent()->IndirectLightingIntensity;
			if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
			{
				SkyLight->GetLightComponent()->SetIndirectLightingIntensity(IndirectLightingIntensity);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_VolumetricScatteringIntensity)
		{
			CacheVolumetricScatteringIntensity = SkyLight->GetLightComponent()->VolumetricScatteringIntensity;
			if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
			{
				SkyLight->GetLightComponent()->SetVolumetricScatteringIntensity(VolumetricScatteringIntensity);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_bLowerHemisphereIsBlack)
		{
			bCacheLowerHemisphereIsBlack = SkyLight->GetLightComponent()->bLowerHemisphereIsBlack;
			if (bCacheLowerHemisphereIsBlack != bLowerHemisphereIsBlack)
			{
				SetLowerHemisphereIsBlack(SkyLight->GetLightComponent(), bLowerHemisphereIsBlack);
				bOverridingLighting |= true;
			}
		}
		if (bOverride_LowerHemisphereColor)
		{
			CacheLowerHemisphereColor = SkyLight->GetLightComponent()->LowerHemisphereColor;
			if (CacheLowerHemisphereColor != LowerHemisphereColor)
			{
				SkyLight->GetLightComponent()->SetLowerHemisphereColor(LowerHemisphereColor);
				bOverridingLighting |= true;
			}
		}
	}
}

void ALocalSkyLightVolume::RestoreLighting()
{
	if (SkyLight.IsValid())
	{
		if (bOverride_bRealTimeCapture)
		{
			if (bCacheRealTimeCapture != bRealTimeCapture)
			{
				SkyLight->GetLightComponent()->SetRealTimeCapture(bCacheRealTimeCapture);
			}
		}
		if (bOverride_SourceType)
		{
			if (CacheSourceType != SourceType)
			{
				SkyLight->GetLightComponent()->SourceType = CacheSourceType;
				SkyLight->GetLightComponent()->MarkRenderStateDirty();
				SkyLight->GetLightComponent()->SetCaptureIsDirty();
			}
		}
		if (bOverride_Cubemap)
		{
			if (CacheCubemap != Cubemap)
			{
				SkyLight->GetLightComponent()->SetCubemap(CacheCubemap);
			}
		}
		if (bOverride_Intensity)
		{
			if (CacheIntensity != Intensity)
			{
				SkyLight->GetLightComponent()->SetIntensity(CacheIntensity);
			}
		}
		if (bOverride_LightColor)
		{
			if (CacheLightColor != LightColor)
			{
				SkyLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
			}
		}
		if (bOverride_IndirectLightingIntensity)
		{
			if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
			{
				SkyLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
			}
		}
		if (bOverride_VolumetricScatteringIntensity)
		{
			if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
			{
				SkyLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
			}
		}
		if (bOverride_bLowerHemisphereIsBlack)
		{
			if (bCacheLowerHemisphereIsBlack != bLowerHemisphereIsBlack)
			{
				SetLowerHemisphereIsBlack(SkyLight->GetLightComponent(), bCacheLowerHemisphereIsBlack);
			}
		}
		if (bOverride_LowerHemisphereColor)
		{
			if (CacheLowerHemisphereColor != LowerHemisphereColor)
			{
				SkyLight->GetLightComponent()->SetLowerHemisphereColor(CacheLowerHemisphereColor);
			}
		}
	}
	bOverridingLighting = false;
}

void ALocalSkyLightVolume::PostRegisterAllComponents()
{
 	Super::PostRegisterAllComponents();
	
	RegisterIntoSubsystem();
}

void ALocalSkyLightVolume::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();
	
	UnregisterFromSubsystem();
}

#if WITH_EDITOR
void ALocalSkyLightVolume::PreEditChange(FProperty* PropertyAboutToChange)
{
	if (PropertyAboutToChange)
	{
		const FString PropertyName = PropertyAboutToChange->GetName();
		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, SkyLight))
		{
			CacheSkyLight = SkyLight;
		}
		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, bRealTimeCapture) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, SourceType) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, Cubemap) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, Intensity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, LightColor) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, IndirectLightingIntensity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, VolumetricScatteringIntensity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, bLowerHemisphereIsBlack) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ALocalSkyLightVolume, LowerHemisphereColor))
		{
			bCacheOverride_bRealTimeCapture = bOverride_bRealTimeCapture;
			bCacheOverride_SourceType = bOverride_SourceType;
			bCacheOverride_Cubemap = bOverride_Cubemap;
			bCacheOverride_Intensity = bOverride_Intensity;
			bCacheOverride_LightColor = bOverride_LightColor;
			bCacheOverride_IndirectLightingIntensity = bOverride_IndirectLightingIntensity;
			bCacheOverride_VolumetricScatteringIntensity = bOverride_VolumetricScatteringIntensity;
			bCacheOverride_bLowerHemisphereIsBlack = bOverride_bLowerHemisphereIsBlack;
			bCacheOverride_LowerHemisphereColor = bOverride_LowerHemisphereColor;
		}
	}
	Super::PreEditChange(PropertyAboutToChange);
}

void ALocalSkyLightVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, SkyLight))
	{
		if (bViewPointInVolume && CacheSkyLight != SkyLight)
		{
			// Restore lighting to previous Sky Light.
			if (CacheSkyLight.IsValid())
			{
				if (bOverride_bRealTimeCapture)
				{
					if (bCacheRealTimeCapture != bRealTimeCapture)
					{
						CacheSkyLight->GetLightComponent()->SetRealTimeCapture(bCacheRealTimeCapture);
					}
				}
				if (bOverride_SourceType)
				{
					if (CacheSourceType != SourceType)
					{
						CacheSkyLight->GetLightComponent()->SourceType = CacheSourceType;
						CacheSkyLight->GetLightComponent()->MarkRenderStateDirty();
						CacheSkyLight->GetLightComponent()->SetCaptureIsDirty();
					}
				}
				if (bOverride_Cubemap)
				{
					if (CacheCubemap != Cubemap)
					{
						CacheSkyLight->GetLightComponent()->SetCubemap(CacheCubemap);
					}
				}
				if (bOverride_Intensity)
				{
					if (CacheIntensity != Intensity)
					{
						CacheSkyLight->GetLightComponent()->SetIntensity(CacheIntensity);
					}
				}
				if (bOverride_LightColor)
				{
					if (CacheLightColor != LightColor)
					{
						CacheSkyLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
					}
				}
				if (bOverride_IndirectLightingIntensity)
				{
					if (CacheIndirectLightingIntensity != IndirectLightingIntensity)
					{
						CacheSkyLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
					}
				}
				if (bOverride_VolumetricScatteringIntensity)
				{
					if (CacheVolumetricScatteringIntensity != VolumetricScatteringIntensity)
					{
						CacheSkyLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
					}
				}
				if (bOverride_bLowerHemisphereIsBlack)
				{
					if (bCacheLowerHemisphereIsBlack != bLowerHemisphereIsBlack)
					{
						SetLowerHemisphereIsBlack(CacheSkyLight->GetLightComponent(), bCacheLowerHemisphereIsBlack);
					}
				}
				if (bOverride_LowerHemisphereColor)
				{
					if (CacheLowerHemisphereColor != LowerHemisphereColor)
					{
						CacheSkyLight->GetLightComponent()->SetLowerHemisphereColor(CacheLowerHemisphereColor);
					}
				}
			}
			OverrideLighting();
		}
	}
	
	if (bViewPointInVolume)
	{
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, bRealTimeCapture))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_bRealTimeCapture)
				{
					if (bOverride_bRealTimeCapture != bCacheOverride_bRealTimeCapture)
					{
						bCacheRealTimeCapture = SkyLight->GetLightComponent()->bRealTimeCapture;
					}
					SkyLight->GetLightComponent()->SetRealTimeCapture(bRealTimeCapture);
				}
				else
				{
					SkyLight->GetLightComponent()->SetRealTimeCapture(bCacheRealTimeCapture);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, SourceType))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_SourceType)
				{
					if (bOverride_SourceType != bCacheOverride_SourceType)
					{
						CacheSourceType = SkyLight->GetLightComponent()->SourceType;
					}
					SkyLight->GetLightComponent()->SourceType = SourceType;
					SkyLight->GetLightComponent()->MarkRenderStateDirty();
					SkyLight->GetLightComponent()->SetCaptureIsDirty();
				}
				else
				{
					SkyLight->GetLightComponent()->SourceType = CacheSourceType;
					SkyLight->GetLightComponent()->MarkRenderStateDirty();
					SkyLight->GetLightComponent()->SetCaptureIsDirty();
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Cubemap))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_Cubemap)
				{
					if (bOverride_Cubemap != bCacheOverride_Cubemap)
					{
						CacheCubemap = SkyLight->GetLightComponent()->Cubemap;
					}
					SkyLight->GetLightComponent()->SetCubemap(Cubemap);
				}
				else
				{
					SkyLight->GetLightComponent()->SetCubemap(CacheCubemap);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Intensity))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_Intensity)
				{
					if (bOverride_Intensity != bCacheOverride_Intensity)
					{
						CacheIntensity = SkyLight->GetLightComponent()->Intensity;
					}
					SkyLight->GetLightComponent()->SetIntensity(Intensity);
				}
				else
				{
					SkyLight->GetLightComponent()->SetIntensity(CacheIntensity);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, LightColor))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_LightColor)
				{
					if (bOverride_LightColor != bCacheOverride_LightColor)
					{
						CacheLightColor = SkyLight->GetLightComponent()->GetLightColor().ToFColor(true);
					}
					SkyLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(LightColor));
				}
				else
				{
					SkyLight->GetLightComponent()->SetLightColor(FLinearColor::FromSRGBColor(CacheLightColor));
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, IndirectLightingIntensity))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_IndirectLightingIntensity)
				{
					if (bOverride_IndirectLightingIntensity != bCacheOverride_IndirectLightingIntensity)
					{
						CacheIndirectLightingIntensity = SkyLight->GetLightComponent()->IndirectLightingIntensity;
					}
					CacheSkyLight->GetLightComponent()->SetIndirectLightingIntensity(IndirectLightingIntensity);
				}
				else
				{
					CacheSkyLight->GetLightComponent()->SetIndirectLightingIntensity(CacheIndirectLightingIntensity);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, VolumetricScatteringIntensity))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_VolumetricScatteringIntensity)
				{
					if (bOverride_VolumetricScatteringIntensity != bCacheOverride_VolumetricScatteringIntensity)
					{
						CacheVolumetricScatteringIntensity = SkyLight->GetLightComponent()->VolumetricScatteringIntensity;
					}
					CacheSkyLight->GetLightComponent()->SetVolumetricScatteringIntensity(VolumetricScatteringIntensity);
				}
				else
				{
					CacheSkyLight->GetLightComponent()->SetVolumetricScatteringIntensity(CacheVolumetricScatteringIntensity);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, bLowerHemisphereIsBlack))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_bLowerHemisphereIsBlack)
				{
					if (bOverride_bLowerHemisphereIsBlack != bCacheOverride_bLowerHemisphereIsBlack)
					{
						bCacheLowerHemisphereIsBlack = SkyLight->GetLightComponent()->bLowerHemisphereIsBlack;
					}
					SetLowerHemisphereIsBlack(SkyLight->GetLightComponent(), bLowerHemisphereIsBlack);
				}
				else
				{
					SetLowerHemisphereIsBlack(SkyLight->GetLightComponent(), bCacheLowerHemisphereIsBlack);
				}
			}
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, LowerHemisphereColor))
		{
			if (SkyLight.IsValid())
			{
				if (bOverride_LowerHemisphereColor)
				{
					if (bOverride_LowerHemisphereColor != bCacheOverride_LowerHemisphereColor)
					{
						CacheLowerHemisphereColor = SkyLight->GetLightComponent()->LowerHemisphereColor;
					}
					SkyLight->GetLightComponent()->SetLowerHemisphereColor(LowerHemisphereColor);
				}
				else
				{
					SkyLight->GetLightComponent()->SetLowerHemisphereColor(CacheLowerHemisphereColor);
				}
			}
		}
	}
}

bool ALocalSkyLightVolume::CanEditChange(const FProperty* InProperty) const
{
	const FName PropertyName = InProperty->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, bRealTimeCapture) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, SourceType) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Cubemap) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Intensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, LightColor) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, IndirectLightingIntensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, VolumetricScatteringIntensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, bLowerHemisphereIsBlack) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, LowerHemisphereColor))
	{
		return SkyLight.IsValid();
	}
	
	return Super::CanEditChange(InProperty);
}
#endif