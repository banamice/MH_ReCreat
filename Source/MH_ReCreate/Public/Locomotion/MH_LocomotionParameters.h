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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDeceleration = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFrictionFactor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFriction = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseSeparateBrakingFriction= true;
	
	
};

//这里指的是姿态循环之外的type。比如说晕厥，下蹲,走路，跑步
UENUM(BlueprintType)
enum class FGaitType: uint8
{
	Walk,
	Run,
	Crouch,
};
