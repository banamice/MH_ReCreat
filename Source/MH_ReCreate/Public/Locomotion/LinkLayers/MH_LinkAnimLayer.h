// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locomotion/MH_BaseAnimInstance.h"
#include "MH_LinkAnimLayer.generated.h"

class UMH_PlayerAnimInstance;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_LinkAnimLayer : public UMH_BaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "MH|Animation", meta = (BlueprintThreadSafe))
	UMH_PlayerAnimInstance* GetPlayerAnimInstance() const;
};
