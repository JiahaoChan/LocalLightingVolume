// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

#pragma once

// Engine Include
#include "CoreMinimal.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/Volume.h"

// Plugins Include
#include "Interface_LocalLightingVolume.h"

// Generated Include
#include "LocalSkyLightVolume.generated.h"

/**
 * Allow to modify Sky Light when View Point in the range of Volume.
 */
UCLASS(AutoExpandCategories = ("Sky Light"), MinimalAPI)
class ALocalSkyLightVolume : public ALocalLightingVolumeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_bRealTimeCapture:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_SourceType:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Cubemap:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Intensity:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_LightColor:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_IndirectLightingIntensity:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_VolumetricScatteringIntensity:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_bLowerHemisphereIsBlack:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_LowerHemisphereColor:1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light")
	TWeakObjectPtr<class ASkyLight> SkyLight;

	/** When enabled, the sky will be captured and convolved to achieve dynamic diffuse and specular environment lighting. 
	 * SkyAtmosphere, VolumetricCloud Components as well as sky domes with Sky materials are taken into account. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light", meta = (EditCondition = "bOverride_bRealTimeCapture"))
	bool bRealTimeCapture;

	/** Indicates where to get the light contribution from. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light", meta = (EditCondition = "bOverride_SourceType"))
	TEnumAsByte<ESkyLightSourceType> SourceType;

	/** Cubemap to use for sky lighting if SourceType is set to SLS_SpecifiedCubemap. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light", meta = (EditCondition = "bOverride_Cubemap"))
	TObjectPtr<UTextureCube> Cubemap;

	/** 
	 * Total energy that the light emits.  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta = (DisplayName = "Intensity Scale", UIMin = "0.0", UIMax = "50000.0", SliderExponent = "10.0", ShouldShowInViewport = true, EditCondition = "bOverride_Intensity"))
	float Intensity;

	/** 
	 * Filter color of the light.
	 * Note that this can change the light's effective intensity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta = (HideAlphaChannel, ShouldShowInViewport = true, EditCondition = "bOverride_LightColor"))
	FColor LightColor;

	/** 
	 * Scales the indirect lighting contribution from this light. 
	 * A value of 0 disables any GI from this light. Default is 1.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta=(UIMin = "0.0", UIMax = "6.0", EditCondition = "bOverride_IndirectLightingIntensity"))
	float IndirectLightingIntensity;

	/** Intensity of the volumetric scattering from this light.  This scales Intensity and LightColor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta=(UIMin = "0.25", UIMax = "4.0", EditCondition = "bOverride_VolumetricScatteringIntensity"))
	float VolumetricScatteringIntensity;

	/** 
	 * Whether all distant lighting from the lower hemisphere should be set to LowerHemisphereColor.  
	 * Enabling this is accurate when lighting a scene on a planet where the ground blocks the sky, 
	 * However disabling it can be useful to approximate skylight bounce lighting (eg Movable light).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light", meta = (DisplayName = "Lower Hemisphere Is Solid Color", EditCondition = "bOverride_bLowerHemisphereIsBlack"))
	bool bLowerHemisphereIsBlack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sky Light", meta = (EditCondition = "bOverride_LowerHemisphereColor"))
	FLinearColor LowerHemisphereColor;

	bool bCacheRealTimeCapture;
	TEnumAsByte<ESkyLightSourceType> CacheSourceType;
	UPROPERTY(Transient)
	TObjectPtr<UTextureCube> CacheCubemap;
	float CacheIntensity;
	FColor CacheLightColor;
	float CacheIndirectLightingIntensity;
	float CacheVolumetricScatteringIntensity;
	bool bCacheLowerHemisphereIsBlack;
	FLinearColor CacheLowerHemisphereColor;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TWeakObjectPtr<ASkyLight> CacheSkyLight;
	uint8 bCacheOverride_bRealTimeCapture:1;
	uint8 bCacheOverride_SourceType:1;
	uint8 bCacheOverride_Cubemap:1;
	uint8 bCacheOverride_Intensity:1;
	uint8 bCacheOverride_LightColor:1;
	uint8 bCacheOverride_IndirectLightingIntensity:1;
	uint8 bCacheOverride_VolumetricScatteringIntensity:1;
	uint8 bCacheOverride_bLowerHemisphereIsBlack:1;
	uint8 bCacheOverride_LowerHemisphereColor:1;
#endif

public:
	ALocalSkyLightVolume();

protected:
	//~ Begin ALocalLightingVolumeBase Interface
	virtual void OverrideLighting() override;
	virtual void RestoreLighting() override;
	//~ End ALocalLightingVolumeBase Interface

public:
    //~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
    //~ End UObject Interface
};
