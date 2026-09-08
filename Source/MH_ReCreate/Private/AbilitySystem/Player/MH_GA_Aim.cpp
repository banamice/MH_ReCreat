// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_Aim.h"

#include "AbilitySystemComponent.h"

UMH_GA_Aim::UMH_GA_Aim()
{
	// 按键保持期间需要保留能力实例，以便保存激活时授予的能力句柄。
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
	// 确保服务器收到松开事件，以便移除临时授予的能力。
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

	// 能力由服务器授予，生成的句柄会随 ASC 复制，并用于确定性清理。
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

	// UE 5.8 的 FGameplayAbilitySpecHandle 没有 Invalidate()，使用默认值重置为无效句柄。
	GrantedAbilityHandle = FGameplayAbilitySpecHandle();
}
