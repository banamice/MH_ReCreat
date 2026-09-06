// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locomotion/MH_CharacterAnimInstance.h"
#include "MH_PlayerAnimInstance.generated.h"

class AMH_BasePlayerCharacter;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_PlayerAnimInstance : public UMH_CharacterAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMH_BasePlayerCharacter> MH_Player;
};
