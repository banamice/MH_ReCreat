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
	if (bIsDebug)
	{
		Debug();
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

void UMH_CharacterAnimInstance::SetGaitType(const FGaitType InStateType)
{
	GaitType = InStateType;
	
}

void UMH_CharacterAnimInstance::Debug()
{
	if (!GEngine)
	{
		return;
	}

	if (!IsValid(MH_Character) || !IsValid(MH_MovementComponent))
	{
		GEngine->AddOnScreenDebugMessage(
			static_cast<uint64>(GetTypeHash(this)),
			0.0f,
			FColor::Red,
			TEXT("Locomotion Debug: character or movement component is unavailable"));
		return;
	}

	const float PlanarSpeed = VelocityXY.Size();
	const float AccelerationMagnitude = Acceleration.Size();
	const FString GaitName = UEnum::GetValueAsString(GaitType);
	const FString MovementModeName = MH_MovementComponent->GetMovementName();

	const FString DebugMessage = FString::Printf(
		TEXT("Locomotion [%s]\n")
		TEXT("Gait: %s | Movement Mode: %s | Grounded: %s | Falling: %s | Crouching: %s\n")
		TEXT("Velocity XYZ: %s | Velocity XY: %s | Planar Speed: %.1f\n")
		TEXT("Acceleration: %s | Magnitude: %.1f\n")
		TEXT("Max Speed: %.1f | Max Acceleration: %.1f | Braking Deceleration: %.1f\n")
		TEXT("Braking Friction: %.2f | Friction Factor: %.2f | Separate Friction: %s"),
		*GetNameSafe(MH_Character.Get()),
		*GaitName,
		*MovementModeName,
		MH_MovementComponent->IsMovingOnGround() ? TEXT("true") : TEXT("false"),
		MH_MovementComponent->IsFalling() ? TEXT("true") : TEXT("false"),
		MH_MovementComponent->IsCrouching() ? TEXT("true") : TEXT("false"),
		*VelocityXYZ.ToCompactString(),
		*VelocityXY.ToCompactString(),
		PlanarSpeed,
		*Acceleration.ToCompactString(),
		AccelerationMagnitude,
		MH_MovementComponent->MaxWalkSpeed,
		MH_MovementComponent->MaxAcceleration,
		MH_MovementComponent->BrakingDecelerationWalking,
		MH_MovementComponent->BrakingFriction,
		MH_MovementComponent->BrakingFrictionFactor,
		MH_MovementComponent->bUseSeparateBrakingFriction ? TEXT("true") : TEXT("false"));

	GEngine->AddOnScreenDebugMessage(
		static_cast<uint64>(GetTypeHash(this)),
		0.0f,
		FColor::Cyan,
		DebugMessage);
}
