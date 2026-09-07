// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_Run.h"

#include "Character/Player/MH_BasePlayerCharacter.h"

UMH_GA_Run::UMH_GA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bRetriggerInstancedAbility = false;
	bReplicateInputDirectly = true;
}

void UMH_GA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AMH_BasePlayerCharacter* Character = GetPlayerCharacter())
	{
		Character->OnGaitTypeChange(FGaitType::Run);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UMH_GA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	RestoreWalkGait();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UMH_GA_Run::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	RestoreWalkGait();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMH_GA_Run::RestoreWalkGait()
{
	if (AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
		Character && Character->GaitType == FGaitType::Run)
	{
		Character->OnGaitTypeChange(FGaitType::Walk);
	}
}
