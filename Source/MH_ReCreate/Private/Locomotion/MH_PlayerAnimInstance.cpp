// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_PlayerAnimInstance.h"

#include "Character/Player/MH_BasePlayerCharacter.h"

void UMH_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MH_Player = Cast<AMH_BasePlayerCharacter>(TryGetPawnOwner());
}
