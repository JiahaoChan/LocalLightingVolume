/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "Interface_LocalLightingVolume.h"

#include "LocalSkyLightVolume.generated.h"

/**
 * Allow to modify Sky Light when View Point in the range of Volume.
 */
UCLASS(autoexpandcategories = LocalSkyLightVolume, hidecategories = (Advanced, Collision, Volume, Brush, Attachment), MinimalAPI)
class ALocalSkyLightVolume : public AVolume, public IInterface_LocalLightingVolume
{
	GENERATED_UCLASS_BODY()
	
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
	TEnumAsByte<enum ESkyLightSourceType> SourceType;
	
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
	
	bool bViewPointInVolume;
	bool bOverridingLighting;
	
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
	//~ Begin IInterface_LocalLightingVolume Interface
	virtual void Process(const FVector& ViewPoint) override;
	virtual bool IsOverridingLighting() override;
	
	virtual void RegisterIntoSubsystem() override;
	virtual void UnregisterFromSubsystem() override;
	//~ End IInterface_LocalLightingVolume Interface
	
protected:
	virtual void OverrideLighting();
	virtual void RestoreLighting();
	
public:
	//~ Begin AActor Interface
	virtual void PostRegisterAllComponents() override;
    virtual void PostUnregisterAllComponents() override;
    //~ End AActor Interface
	
#if WITH_EDITOR
	FDelegateHandle PreSaveHandle;
	void OnOverridingLightComponentPackagePreSave(UPackage* Package, FObjectPreSaveContext Context);
	FDelegateHandle SavedHandle;
	void OnOverridingLightComponentPackageSaved(const FString& FileName, UPackage* Package, FObjectPostSaveContext Context);
#endif
	
    //~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
    //~ End UObject Interface
};