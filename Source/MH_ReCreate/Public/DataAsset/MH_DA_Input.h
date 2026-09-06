// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "MH_DA_Input.generated.h"

/**
 * Input action mapping data used by the project.
 */

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputActions
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input",meta=(categories="InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputAction;
	
	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
};

UCLASS()
class MH_RECREATE_API UMH_DA_Input : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	//移动按键输入
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input",meta=(TitleProperty="InputTag"))
	TArray<FInputActions> InputActions;

	UInputAction* FindInputActionByTag(const FGameplayTag& InputTag);
	//能力按键输入
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input",meta=(TitleProperty="InputTag"))
	TArray<FInputActions> AbilityInputActions;
};
