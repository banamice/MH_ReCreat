// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_PlayerGameplayAbility.h"

#include "BPFuncLib/MH_BluePrintFuncLib.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/CombatComponent/MH_PlayerCombatComponent.h"
#include "Core/MH_BasePlayerController.h"

AMH_BasePlayerCharacter* UMH_PlayerGameplayAbility::GetPlayerCharacter()
{
	if (!PlayerCharacter.IsValid())
	{
		PlayerCharacter = Cast<AMH_BasePlayerCharacter>(GetAvatarActorFromActorInfo());
	}

	return PlayerCharacter.Get();
}

AMH_BasePlayerController* UMH_PlayerGameplayAbility::GetPlayerController()
{
	if (!PlayerController.IsValid())
	{
		PlayerController = Cast<AMH_BasePlayerController>(
			CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr);
	}

	return PlayerController.Get();
}

UMH_PlayerCombatComponent* UMH_PlayerGameplayAbility::GetPlayerCombatComponent()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
	{
		UE_LOG(LogMH, Warning, TEXT("%s: GetPlayerCombatComponent failed because AvatarActor is invalid"), *GetName());
		return nullptr;
	}

	if (const AMH_BasePlayerCharacter* Character = Cast<AMH_BasePlayerCharacter>(AvatarActor))
	{
		if (UMH_PlayerCombatComponent* CombatComponent = Character->GetMHCombatComponent(); IsValid(CombatComponent))
		{
			return CombatComponent;
		}
	}

	if (UMH_PlayerCombatComponent* CombatComponent = AvatarActor->FindComponentByClass<UMH_PlayerCombatComponent>();
		IsValid(CombatComponent))
	{
		return CombatComponent;
	}

	UE_LOG(LogMH, Error, TEXT("%s: Avatar %s has no UMH_PlayerCombatComponent (class: %s)"),
		*GetName(), *AvatarActor->GetName(), *AvatarActor->GetClass()->GetName());
	return nullptr;
}
