// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_PlayerGameplayAbility.h"
#include "MH_GA_Crouch.generated.h"

// 切换 Walk/Crouch 基础姿态的输入能力。
UCLASS()
class MH_RECREATE_API UMH_GA_Crouch : public UMH_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMH_GA_Crouch();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
};
