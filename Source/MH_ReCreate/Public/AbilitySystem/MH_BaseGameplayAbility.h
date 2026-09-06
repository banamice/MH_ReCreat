// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MH_BaseGameplayAbility.generated.h"

class UMH_BaseAbilitySystemComponent;
class UMH_PawnCombatConponent;

UENUM(BlueprintType)
enum class EActiveType : uint8
{
	OnTrigger,
	OnGiven
};

/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_BaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Ability",meta = (AllowPrivateAccess = "true"))
	EActiveType ActiveType = EActiveType::OnTrigger;
	
	UFUNCTION(BlueprintPure, Category = "MH|Ability")
	UMH_PawnCombatConponent* GetCombatComponent() const;
	UFUNCTION(BlueprintPure, Category = "MH|Ability")
	UMH_BaseAbilitySystemComponent* GetUMH_BaseAbilitySystemComponent() const;
};
