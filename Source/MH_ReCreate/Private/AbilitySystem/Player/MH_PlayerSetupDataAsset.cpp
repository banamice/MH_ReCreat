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
		
		FGameplayAbilitySpec Spec(Ability.Ability, AbilityLevel);
		Spec.SourceObject = InAsc->GetAvatarActor();
		Spec.Level = AbilityLevel;
		//不一定需要添加tag、战斗状态其实不依靠inputTag激活能力。只有非战斗状态，比如说家园里的翔虫之类的没有固定状态的。
		//使用输入绑定激活
		if (Ability.GameplayTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Ability.GameplayTag);
		}
		InAsc->GiveAbility(Spec); 
	}
}

