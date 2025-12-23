// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

#pragma once

// Engine Include
#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

// Plugins Include
#include "Interface_LocalLightingVolume.h"

// Generated Include
#include "LocalDirectionalLightVolume.generated.h"

/**
 * Allow to modify Directional Light when View Point in the range of Volume.
 */
UCLASS(AutoExpandCategories = ("Directional Light"), MinimalAPI)
class ALocalDirectionalLightVolume : public ALocalLightingVolumeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Rotation:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_Intensity:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_LightColor:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_IndirectLightingIntensity:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta=(PinHiddenByDefault, InlineEditConditionToggle))
	uint8 bOverride_VolumetricScatteringIntensity:1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Directional Light")
	TWeakObjectPtr<class ADirectionalLight> DirectionalLight;

	/** 
	 * Directional Light World Rotation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Directional Light", meta = (EditCondition = "bOverride_Rotation"))
	FRotator Rotation;

	/** 
	 * Maximum illumination from the light in lux.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Directional Light", meta = (DisplayName = "Intensity", UIMin = "0.0", UIMax = "150.0", SliderExponent = "2.0", Units = "lux", ShouldShowInViewport = true, EditCondition = "bOverride_Intensity"))
	float Intensity;

	/** 
	 * Filter color of the light.
	 * Note that this can change the light's effective intensity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Directional Light", meta = (HideAlphaChannel, ShouldShowInViewport = true, EditCondition = "bOverride_LightColor"))
	FColor LightColor;

	/** 
	 * Scales the indirect lighting contribution from this light. 
	 * A value of 0 disables any GI from this light. Default is 1.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Directional Light", meta=(UIMin = "0.0", UIMax = "6.0", EditCondition = "bOverride_IndirectLightingIntensity"))
	float IndirectLightingIntensity;

	/** Intensity of the volumetric scattering from this light.  This scales Intensity and LightColor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Directional Light", meta=(UIMin = "0.25", UIMax = "4.0", EditCondition = "bOverride_VolumetricScatteringIntensity"))
	float VolumetricScatteringIntensity;

	FRotator CacheRotation;
	float CacheIntensity;
	FColor CacheLightColor;
	float CacheIndirectLightingIntensity;
	float CacheVolumetricScatteringIntensity;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TWeakObjectPtr<ADirectionalLight> CacheDirectionalLight;
	uint8 bCacheOverride_Rotation:1;
	uint8 bCacheOverride_Intensity:1;
	uint8 bCacheOverride_LightColor:1;
	uint8 bCacheOverride_IndirectLightingIntensity:1;
	uint8 bCacheOverride_VolumetricScatteringIntensity:1;
#endif

public:
	ALocalDirectionalLightVolume();

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
