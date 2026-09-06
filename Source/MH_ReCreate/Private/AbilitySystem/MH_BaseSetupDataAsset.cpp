// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MH_BaseSetupDataAsset.h"

#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "AbilitySystem/MH_BaseGameplayAbility.h"


void UMH_BaseSetupDataAsset::GiveAbilitiesToASC(UMH_BaseAbilitySystemComponent* InAsc, int32 AbilityLevel)
{
	check(InAsc);
	GiveAbilitiesFromArray(InAsc, OnGivenAbilities, AbilityLevel);
	GiveAbilitiesFromArray(InAsc, ReactAbilities, AbilityLevel);
}

void UMH_BaseSetupDataAsset::GiveAbilitiesFromArray(UMH_BaseAbilitySystemComponent* InAsc,
	const TArray<TSubclassOf<UMH_BaseGameplayAbility>>& Abilities, int32 AbilityLevel)
{
	if (Abilities.Num() == 0) return;
	for (const auto& Ability : Abilities)
	{
		if (!Ability)
		{
			continue;
		}

		 FGameplayAbilitySpec Spec(Ability, AbilityLevel);
		Spec.SourceObject = InAsc->GetAvatarActor();
		Spec.Level = AbilityLevel;
		InAsc->GiveAbility(Spec); 
	}
}
