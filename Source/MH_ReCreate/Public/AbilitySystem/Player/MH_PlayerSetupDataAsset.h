// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/MH_BaseSetupDataAsset.h"
#include "MH_PlayerSetupDataAsset.generated.h"


class UMH_PlayerGameplayAbility;

USTRUCT(BlueprintType)
struct FInputAbility
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FGameplayTag GameplayTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TSubclassOf<UMH_PlayerGameplayAbility> Ability;
	
	bool IsValid() const;
	
};
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_PlayerSetupDataAsset : public UMH_BaseSetupDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void GiveAbilitiesToASC(UMH_BaseAbilitySystemComponent* InAsc,int32 AbilityLevel = 1) override;

	
	//带有输入才能激活的能力与对应标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|InputAbility",meta=(TitleProperty="GameplayTag"))
	TArray<FInputAbility> InputAbility;	
};
