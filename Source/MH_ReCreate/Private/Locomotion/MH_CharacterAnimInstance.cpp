// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_CharacterAnimInstance.h"

#include "Character/Player/MH_BasePlayerCharacter.h"

void UMH_CharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MH_Character = Cast<AMH_BaseCharacter>(TryGetPawnOwner());
	if (MH_Character)
	{
		MH_MovementComponent = MH_Character->GetCharacterMovement();
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
