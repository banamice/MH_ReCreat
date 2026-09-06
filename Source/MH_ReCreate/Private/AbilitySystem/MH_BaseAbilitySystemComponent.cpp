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
		if (!Ability.DynamicAbilityTags.HasTagExact(AbilityTag)) continue;
		
		TryActivateAbility(Ability.Handle);
	}
}

void UMH_BaseAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& AbilityTag)
{
}

