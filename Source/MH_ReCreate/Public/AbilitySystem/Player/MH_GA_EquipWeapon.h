// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_PlayerGameplayAbility.h"
#include "MH_GA_EquipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_GA_EquipWeapon : public UMH_PlayerGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> EquipMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FGameplayTag EventTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FGameplayTag WeaponTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FName HoldSocketName;
	
	UFUNCTION()
	void OnMontageEnd();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
};
