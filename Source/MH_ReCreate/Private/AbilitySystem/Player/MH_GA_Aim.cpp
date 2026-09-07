// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_Aim.h"

#include "AbilitySystemComponent.h"

UMH_GA_Aim::UMH_GA_Aim()
{
	// The ability instance must persist while the button is held so it can retain
	// the handle of the ability granted on activation.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
	// Ensure the server receives the release event so it can remove the granted GA.
	bReplicateInputDirectly = true;
}

void UMH_GA_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!IsValid(ASC) || !AbilityToGrant || !GrantAbilityTag.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Gameplay abilities are granted by the authority. The resulting handle is
	// replicated with the ASC and is retained for deterministic cleanup.
	if (ASC->GetOwner() && ASC->GetOwner()->HasAuthority() && !GrantedAbilityHandle.IsValid())
	{
		FGameplayAbilitySpec Spec(AbilityToGrant, 1);
		Spec.SourceObject = ASC->GetAvatarActor();
		Spec.GetDynamicSpecSourceTags().AddTag(GrantAbilityTag);
		GrantedAbilityHandle = ASC->GiveAbility(Spec);
	}
}

void UMH_GA_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	ClearGrantedAbility(ActorInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UMH_GA_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearGrantedAbility(ActorInfo);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMH_GA_Aim::ClearGrantedAbility(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!GrantedAbilityHandle.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (IsValid(ASC) && ASC->GetOwner() && ASC->GetOwner()->HasAuthority())
	{
		ASC->ClearAbility(GrantedAbilityHandle);
	}

	// FGameplayAbilitySpecHandle has no Invalidate() API in UE 5.8;
	// assigning its default value resets it to the invalid INDEX_NONE state.
	GrantedAbilityHandle = FGameplayAbilitySpecHandle();
}
