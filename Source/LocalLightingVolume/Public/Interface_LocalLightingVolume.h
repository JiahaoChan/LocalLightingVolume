/**
 * Plugin LocalLightingVolume
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 * Copyright TA.Jiahao.Chan, Inc. All Rights Reserved.
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Interface_LocalLightingVolume.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInterface_LocalLightingVolume : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInterface_LocalLightingVolume
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void Process(const FVector& ViewPoint) = 0;
	virtual bool IsOverridingLighting() = 0;
	
	virtual void RegisterIntoSubsystem() = 0;
	virtual void UnregisterFromSubsystem() = 0;
};