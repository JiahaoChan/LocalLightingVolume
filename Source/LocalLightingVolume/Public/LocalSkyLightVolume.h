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
UCLASS(autoexpandcategories = LocalLightingVolume, hidecategories = (Advanced, Collision, Volume, Brush, Attachment), MinimalAPI)
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta = (DisplayName = "Intensity", UIMin = "0.0", UIMax = "20.0", ShouldShowInViewport = true, EditCondition = "bOverride_Intensity"))
	float Intensity;
	
	/** 
	 * Filter color of the light.
	 * Note that this can change the light's effective intensity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, interp, Category = "Sky Light", meta = (HideAlphaChannel, ShouldShowInViewport = true, EditCondition = "bOverride_LightColor"))
	FColor LightColor;
	
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
	
	UPROPERTY(Transient)
	TWeakObjectPtr<ASkyLight> CacheSkyLight;
	bool bCacheRealTimeCapture;
	TEnumAsByte<ESkyLightSourceType> CacheSourceType;
	UPROPERTY(Transient)
	TObjectPtr<UTextureCube> CacheCubemap;
	float CacheIntensity;
	FColor CacheLightColor;
	bool bCacheLowerHemisphereIsBlack;
	FLinearColor CacheLowerHemisphereColor;
	
	uint8 bCacheOverride_bRealTimeCapture:1;
	uint8 bCacheOverride_SourceType:1;
	uint8 bCacheOverride_Cubemap:1;
	uint8 bCacheOverride_Intensity:1;
	uint8 bCacheOverride_LightColor:1;
	uint8 bCacheOverride_bLowerHemisphereIsBlack:1;
	uint8 bCacheOverride_LowerHemisphereColor:1;
	
public:
	//~ Begin IInterface_LocalLightingVolume Interface
	virtual void Process(const FVector& ViewPoint) override;
	virtual bool IsOverridingLighting() override;
	
	virtual void RegisterIntoSubsystem() override;
	virtual void UnregisterFromSubsystem() override;
	//~ End IInterface_LocalLightingVolume Interface
	
private:
	void OverrideLighting();
	void RestoreLighting();
	
public:
	//~ Begin AActor Interface
	virtual void PostRegisterAllComponents() override;
    virtual void PostUnregisterAllComponents() override;
    //~ End AActor Interface
	
    //~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
    //~ End UObject Interface
};