/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
 */

#include "LocalSkyLightVolume.h"

#include "Components/BrushComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/SkyLight.h"

#include "LocalLightingSubsystem.h"

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
	bLowerHemisphereIsBlack = true;
	LowerHemisphereColor = FLinearColor::Black;
	
	bViewPointInVolume = false;
	bOverridingLighting = false;
	
	bCacheRealTimeCapture = false;
	CacheSourceType = SLS_CapturedScene;
	CacheCubemap = nullptr;
	CacheIntensity = 1.0f;
	CacheLightColor = FColor::White;
	bCacheLowerHemisphereIsBlack = true;
	CacheLowerHemisphereColor = FLinearColor::Black;
}

void ALocalSkyLightVolume::Process(const FVector& ViewPoint)
{
	bool bViewPointInVolumeLastTime = bViewPointInVolume;
	bViewPointInVolume = EncompassesPoint(ViewPoint);
	if (bViewPointInVolumeLastTime != bViewPointInVolume)
	{
		if (bViewPointInVolume)
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
				if (bOverride_bLowerHemisphereIsBlack)
				{
					bCacheLowerHemisphereIsBlack = SkyLight->GetLightComponent()->bLowerHemisphereIsBlack;
					if (bCacheLowerHemisphereIsBlack != bLowerHemisphereIsBlack)
					{
						SkyLight->GetLightComponent()->bLowerHemisphereIsBlack = bLowerHemisphereIsBlack;
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
		else
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
				if (bOverride_bLowerHemisphereIsBlack)
				{
					if (bCacheLowerHemisphereIsBlack != bLowerHemisphereIsBlack)
					{
						SkyLight->GetLightComponent()->bLowerHemisphereIsBlack = bCacheLowerHemisphereIsBlack;
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
void ALocalSkyLightVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool ALocalSkyLightVolume::CanEditChange(const FProperty* InProperty) const
{
	const FName PropertyName = InProperty->GetFName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, bRealTimeCapture) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, SourceType) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Cubemap) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, Intensity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALocalSkyLightVolume, LightColor))
	{
		return SkyLight.IsValid();
	}
	
	return Super::CanEditChange(InProperty);
}
#endif