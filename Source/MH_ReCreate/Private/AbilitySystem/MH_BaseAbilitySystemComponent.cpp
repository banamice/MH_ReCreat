// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"


// Sets default values for this component's properties
UMH_BaseAbilitySystemComponent::UMH_BaseAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMH_BaseAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& AbilityTag)
{
	if (!AbilityTag.IsValid()) return;
	for (const auto& Ability: GetActivatableAbilities())
	{
		if (!Ability.GetDynamicSpecSourceTags().HasTagExact(AbilityTag)) continue;
		
		TryActivateAbility(Ability.Handle);
	}
}

void UMH_BaseAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& AbilityTag)
{
	if (!AbilityTag.IsValid()) return;

	// Forward the release to matching ability specs. This drives
	// UGameplayAbility::InputReleased (and abilities such as MH_GA_Aim).
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Ability : GetActivatableAbilities())
	{
		if (Ability.GetDynamicSpecSourceTags().HasTagExact(AbilityTag))
		{
			if (Ability.Ability && Ability.IsActive())
			{
				if (Ability.Ability->bReplicateInputDirectly && !IsOwnerActorAuthoritative())
				{
					ServerSetInputReleased(Ability.Handle);
				}

				AbilitySpecInputReleased(Ability);
				const FPredictionKey PredictionKey = Ability.GetPrimaryInstance()
					? Ability.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey()
					: FPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,
					Ability.Handle, PredictionKey);
			}
		}
	}
}

