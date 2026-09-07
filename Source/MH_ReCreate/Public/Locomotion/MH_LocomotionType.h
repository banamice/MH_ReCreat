// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MH_LocomotionType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGaitType: uint8
{
	Idle,
	Run
};
