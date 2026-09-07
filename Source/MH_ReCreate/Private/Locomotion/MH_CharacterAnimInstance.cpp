// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_CharacterAnimInstance.h"

#include "Character/Player/MH_BasePlayerCharacter.h"

void UMH_CharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	NativeUpdateAnimation(0.0f);
}

void UMH_CharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AMH_BaseCharacter* Character = Cast<AMH_BaseCharacter>(TryGetPawnOwner());
	if (MH_Character != Character)
	{
		MH_Character = Character;
		MH_MovementComponent = Character ? Character->GetCharacterMovement() : nullptr;
	}
}

void UMH_CharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!MH_MovementComponent || !MH_Character) return;
	
	VelocityXYZ = MH_MovementComponent->Velocity;
	VelocityXY = FVector(VelocityXYZ.X,VelocityXYZ.Y,0.0f);
	Acceleration = MH_MovementComponent->GetCurrentAcceleration();
}

void UMH_CharacterAnimInstance::SetGaitType(const EGaitType InGaitType) 
{
	GaitType = InGaitType;
}

