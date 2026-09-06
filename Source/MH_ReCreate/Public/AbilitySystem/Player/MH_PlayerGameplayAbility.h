// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MH_BaseGameplayAbility.h"
#include "MH_PlayerGameplayAbility.generated.h"

class UMH_PlayerCombatComponent;
class AMH_BasePlayerController;
class AMH_BasePlayerCharacter;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_PlayerGameplayAbility : public UMH_BaseGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "MH|Ability")
	AMH_BasePlayerCharacter* GetPlayerCharacter();
	UFUNCTION(BlueprintCallable, Category = "MH|Ability")
	AMH_BasePlayerController* GetPlayerController();
	UFUNCTION(BlueprintCallable, Category = "MH|Ability")
	UMH_PlayerCombatComponent* GetPlayerCombatComponent();
private:
	TWeakObjectPtr<AMH_BasePlayerCharacter> PlayerCharacter;
	TWeakObjectPtr<AMH_BasePlayerController> PlayerController;
};
