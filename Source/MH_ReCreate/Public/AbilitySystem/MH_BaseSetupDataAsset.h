// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MH_BaseSetupDataAsset.generated.h"

class UMH_BaseAbilitySystemComponent;
class UMH_BaseGameplayAbility;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_BaseSetupDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "MH|SetupDataAsset")
	virtual void GiveAbilitiesToASC(UMH_BaseAbilitySystemComponent* InAsc,int32 AbilityLevel = 1);
	
	virtual void GiveAbilitiesFromArray(UMH_BaseAbilitySystemComponent* InAsc,const TArray<TSubclassOf<UMH_BaseGameplayAbility>>& Abilities,int32 AbilityLevel);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UMH_BaseGameplayAbility>> OnGivenAbilities;
	
	//受击反应相关能力或者死亡
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UMH_BaseGameplayAbility>> ReactAbilities;
	
};
