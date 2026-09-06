// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/MH_DA_Input.h"

UInputAction* UMH_DA_Input::FindInputActionByTag(const FGameplayTag& InputTag)
{
	for (auto& InputAction : InputActions)
	{
		if (InputAction.InputTag == InputTag)
		{
			return InputAction.InputAction.Get();
		}
	}
	return nullptr;
}
