// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

/**
 * Plugin LocalLightingVolume:
 *		Allow to modify global Light Component such as Sky Light & Directional Light when View Point in the range of Volume.
 */

#pragma once

// Engine Include
#include "Modules/ModuleManager.h"

class FLocalLightingVolumeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
