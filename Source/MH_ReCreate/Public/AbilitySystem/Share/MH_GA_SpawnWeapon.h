// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MH_BaseGameplayAbility.h"
#include "MH_GA_SpawnWeapon.generated.h"

class AMH_ChracterWeapon;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_GA_SpawnWeapon : public UMH_BaseGameplayAbility
{
	GENERATED_BODY()

public:
	UMH_GA_SpawnWeapon();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;	

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	TSubclassOf<AMH_ChracterWeapon> WeaponClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FName SpawnSocketName; 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	FGameplayTag WeaponTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	bool RegisterHold;
};
