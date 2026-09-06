// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MH_BaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "BPFuncLib/MH_BluePrintFuncLib.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"

void UMH_BaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActiveType == EActiveType::OnGiven
		&& ActorInfo
		&& ActorInfo->AvatarActor.IsValid()
		&& ActorInfo->AvatarActor->HasAuthority()
		&& Spec.Handle.IsValid()
		&& !Spec.IsActive())
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UMH_BaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActiveType == EActiveType::OnGiven )
	{
		if (ActorInfo->AvatarActor->HasAuthority())
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UMH_PawnCombatConponent* UMH_BaseGameplayAbility::GetCombatComponent() const
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		return AvatarActor->FindComponentByClass<UMH_PawnCombatConponent>();
	}

	UE_LOG(LogMH, Warning, TEXT("%s: GetCombatComponent failed because AvatarActor is invalid"), *GetName());
	return nullptr;
}

UMH_BaseAbilitySystemComponent* UMH_BaseGameplayAbility::GetUMH_BaseAbilitySystemComponent() const
{
	return Cast<UMH_BaseAbilitySystemComponent>(GetActorInfo().AbilitySystemComponent);
}
