/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "Interface_LocalLightingVolume.h"

#include "LocalDirectionalLightVolume.generated.h"

/**
 * Allow to modify Directional Light when View Point in the range of Volume.
 */
UCLASS(autoexpandcategories = LocalDirectionalLightVolume, hidecategories = (Advanced, Collision, Volume, Brush, Attachment), MinimalAPI)
class ALocalDirectionalLightVolume : public AVolume, public IInterface_LocalLightingVolume
{
	GENERATED_UCLASS_BODY()
	
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
	
	bool bViewPointInVolume;
	bool bOverridingLighting;
	
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
	//~ Begin IInterface_LocalLightingVolume Interface
	virtual void Process(const FVector& ViewPoint) override;
	virtual bool IsOverridingLighting() override;
	//~ End IInterface_LocalLightingVolume Interface
	
private:
	void RegisterIntoSubsystem();
	void UnregisterFromSubsystem();
	
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