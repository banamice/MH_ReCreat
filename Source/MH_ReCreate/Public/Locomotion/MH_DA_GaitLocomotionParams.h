// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_LocomotionParameters.h"
#include "Engine/DataAsset.h"
#include "MH_DA_GaitLocomotionParams.generated.h"

/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_DA_GaitLocomotionParams : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Gait")
	TMap<FGaitType,FLocomotionParameters> GaitParams;
};
