// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class FQRcodeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};