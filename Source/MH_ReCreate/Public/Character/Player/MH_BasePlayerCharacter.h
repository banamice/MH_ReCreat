// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Locomotion/MH_LocomotionParameters.h"
#include "MH_ReCreate/Public/Character/MH_BaseCharacter.h"
#include "MH_BasePlayerCharacter.generated.h"

class UMH_DA_GaitLocomotionParams;
class UMH_PlayerCombatComponent;
struct FInputActionValue;
class UMH_DA_Input;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MH_RECREATE_API AMH_BasePlayerCharacter : public AMH_BaseCharacter
{
	GENERATED_BODY()

public:
	AMH_BasePlayerCharacter();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

protected:
	
	
private:
#pragma  region Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_PlayerCombatComponent> CombatComponent;
#pragma endregion
	
#pragma region Locomotion
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Lcocomotion",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_DA_GaitLocomotionParams> LocomotionParams;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	FGaitType GaitType = FGaitType::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	FGaitType BaseGaitType = FGaitType::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsRunning = false;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	EMoveState MoveState = EMoveState::None;

	void OnGaitTypeChange(const FGaitType InGaitType);
	bool SetBaseGaitType(const FGaitType InBaseGaitType);
	bool SetRunState(bool bInRunning);
	FGaitType GetBaseGaitType() const { return BaseGaitType; }
	bool IsRunning() const { return bIsRunning; }

private:
	bool ApplyLocomotionState(const FGaitType InBaseGaitType, const EMoveState InMoveState);
#pragma endregion
	
#pragma  region Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_DA_Input> InputDA;
	
	void BindInput(UInputComponent* PlayerInputComponent);
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void AbilityPressed(FGameplayTag AbilityTag);
	void AbilityReleased(FGameplayTag AbilityTag);
#pragma endregion
#pragma region Getter Setter
public:
	UFUNCTION(BlueprintPure, Category = "MH|Combat")
	UMH_PlayerCombatComponent* GetMHCombatComponent() const;
#pragma endregion
};
