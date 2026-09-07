// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/MH_PlayerAnimInterface.h"
#include "Locomotion/MH_CharacterAnimInstance.h"
#include "MH_PlayerAnimInstance.generated.h"

class AMH_BasePlayerCharacter;
class UMH_PlayerCombatComponent;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_PlayerAnimInstance : public UMH_CharacterAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMH_BasePlayerCharacter> MH_Player;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_PlayerCombatComponent> MH_CombatComponent;
};
