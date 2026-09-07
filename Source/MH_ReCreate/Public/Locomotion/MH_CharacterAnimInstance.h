// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BaseAnimInstance.h"
#include "MH_LocomotionParameters.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/MH_PlayerAnimInterface.h"
#include "MH_CharacterAnimInstance.generated.h"

class AMH_BaseCharacter;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_CharacterAnimInstance : public UMH_BaseAnimInstance,public IMH_PlayerAnimInterface
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	//Start Interface From
	virtual void SetGaitType(const FGaitType InStateType) override;

#pragma region Debug
public:
	UPROPERTY(BlueprintReadOnly, Category = "MH|Debug", meta = (AllowPrivateAccess = "true"))
	bool bIsDebug = false;
	void Debug();
#pragma endregion

#pragma region DataForLcocomotion
public:
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMH_BaseCharacter> MH_Character;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> MH_MovementComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	FGaitType GaitType = FGaitType::Walk;
	
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector VelocityXY;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector VelocityXYZ;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector Acceleration;
#pragma endregion
};
