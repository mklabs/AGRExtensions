// Copyright 2021 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AGRExtensionsSettings.generated.h"

/**
* Settings for the AGR Extensions Plugin.
*/
UCLASS(Config="Game", defaultconfig, meta=(DisplayName="AGR Extensions"))
class AGREXTENSIONS_API UAGRExtensionsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Config, Category="Inputs")
	FKey ActivationKey;

};
