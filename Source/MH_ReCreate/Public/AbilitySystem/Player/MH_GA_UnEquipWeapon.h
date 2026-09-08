// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_PlayerGameplayAbility.h"
#include "MH_GA_UnEquipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_GA_UnEquipWeapon : public UMH_PlayerGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> UnEquipMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FGameplayTag EventTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FName CarrySocketName;
	
	UFUNCTION()
	void OnMontageEnd();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
};
