// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/LinkLayers/MH_LinkAnimLayer.h"

#include "Locomotion/MH_PlayerAnimInstance.h"

UMH_PlayerAnimInstance* UMH_LinkAnimLayer::GetPlayerAnimInstance() const
{
	const USkeletalMeshComponent* OwningComponent = GetOwningComponent();
	return OwningComponent
		? Cast<UMH_PlayerAnimInstance>(OwningComponent->GetAnimInstance())
		: nullptr;
}
