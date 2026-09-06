// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_PlayerSetupDataAsset.h"

#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "AbilitySystem/Player/MH_PlayerGameplayAbility.h"

bool FInputAbility::IsValid() const
{
	return GameplayTag.IsValid() && Ability;
}

void UMH_PlayerSetupDataAsset::GiveAbilitiesToASC(UMH_BaseAbilitySystemComponent* InAsc, int32 AbilityLevel)
{
	Super::GiveAbilitiesToASC(InAsc, AbilityLevel);
	for (const auto& Ability : InputAbility)
	{
		if (!Ability.IsValid()) continue;
		
		FGameplayAbilitySpec Spec(Ability.Ability, AbilityLevel);
		Spec.SourceObject = InAsc->GetAvatarActor();
		Spec.Level = AbilityLevel;
		Spec.DynamicAbilityTags.AddTag(Ability.GameplayTag);
		InAsc->GiveAbility(Spec);
	}
}

