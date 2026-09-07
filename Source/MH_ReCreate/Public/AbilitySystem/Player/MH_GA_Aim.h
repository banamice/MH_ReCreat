// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_PlayerGameplayAbility.h"
#include "MH_GA_Aim.generated.h"

/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_GA_Aim : public UMH_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMH_GA_Aim();
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	/** Ability granted while the aim input is held. It is removed when the input is released. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> AbilityToGrant;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTag GrantAbilityTag;

	FGameplayAbilitySpecHandle GrantedAbilityHandle;

	void ClearGrantedAbility(const FGameplayAbilityActorInfo* ActorInfo);
};
