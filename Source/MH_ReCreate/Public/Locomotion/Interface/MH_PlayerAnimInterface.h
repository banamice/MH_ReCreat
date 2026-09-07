// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locomotion/MH_LocomotionType.h"
#include "UObject/Interface.h"
#include "MH_PlayerAnimInterface.generated.h"

UINTERFACE()
class UMH_PlayerAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MH_RECREATE_API IMH_PlayerAnimInterface
{
	GENERATED_BODY()

public:
	virtual void SetGaitType(const EGaitType InGaitType) = 0;
};
