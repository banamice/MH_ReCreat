// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MH_ReCreate/Public/Character/MH_BaseCharacter.h"
#include "MH_BasePlayerCharacter.generated.h"

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
