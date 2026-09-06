// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MH_BaseAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_BaseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMH_BaseAbilitySystemComponent();
	
	void OnAbilityInputPressed(const FGameplayTag& AbilityTag);
	void OnAbilityInputReleased(const FGameplayTag& AbilityTag);

};
