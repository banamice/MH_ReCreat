// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "DataAsset/MH_DA_Input.h"
#include "MH_EnhanceInputComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_EnhanceInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UMH_EnhanceInputComponent();

	template<class UserObject, typename FUNC>
	void BindActionFromDA( UMH_DA_Input* InputDA,
		const FGameplayTag& InputTag,
		UserObject* UserObj,
		FUNC Func,
		ETriggerEvent TriggerEvent);
	
	//TODO:这里只有按下和松开回调，后续可能会需要增加
	template<class UserObject, typename FUNC>
	void BindActionFromDA( UMH_DA_Input* InputDA,
		UserObject* UserObj,
		FUNC PressedFunc,
		FUNC ReleasedFunc);
	

};

template <class UserObject, typename FUNC>
void UMH_EnhanceInputComponent::BindActionFromDA(
	UMH_DA_Input* InputDA,
	const FGameplayTag& InputTag,
	UserObject* UserObj,
	FUNC Func,
	ETriggerEvent TriggerEvent)
{
	checkf(InputDA, TEXT("InputDA is nullptr"));
	checkf(UserObj, TEXT("UserObj is nullptr"));

	if (UInputAction* InputAction = InputDA->FindInputActionByTag(InputTag))
	{
		BindAction(InputAction, TriggerEvent, UserObj, Func);
	}
}

template <class UserObject, typename FUNC>
void UMH_EnhanceInputComponent::BindActionFromDA(UMH_DA_Input* InputDA, UserObject* UserObj, FUNC PressedFunc,
	FUNC ReleasedFunc)
{
	checkf(InputDA, TEXT("InputDA is nullptr"));
	checkf(UserObj, TEXT("UserObj is nullptr"));
	for (auto AbilityInput :InputDA->AbilityInputActions)
	{
		if (!AbilityInput.IsValid()) continue;
		BindAction(AbilityInput.InputAction.Get(), ETriggerEvent::Started, UserObj, PressedFunc, AbilityInput.InputTag);
		BindAction(AbilityInput.InputAction.Get(), ETriggerEvent::Completed, UserObj, ReleasedFunc, AbilityInput.InputTag);
	}
}


