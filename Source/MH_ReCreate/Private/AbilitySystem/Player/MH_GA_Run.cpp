// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Player/MH_GA_Run.h"

#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"

UMH_GA_Run::UMH_GA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bRetriggerInstancedAbility = false;
	bReplicateInputDirectly = true;
}

void UMH_GA_Run::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 持有武器时不允许奔跑。
	const UMH_PawnCombatConponent* CombatComponent = GetCombatComponent();
	if (CombatComponent && CombatComponent->HoldWeaponTag.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	if (!Character || !Character->SetRunState(true))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UMH_GA_Run::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UMH_GA_Run::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	RestoreBaseGait();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMH_GA_Run::RestoreBaseGait()
{
	AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	if (Character && Character->IsRunning())
	{
		Character->SetRunState(false);
	}
}
