// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MH_LocomotionParameters.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLocomotionParameters
{
	GENERATED_BODY()
	
public:
	//还有很多
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 100.f;
};