// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_CharacterAnimInstance.h"

#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

namespace MHLocomotionDebug
{
	constexpr float MessageDuration = 0.0f;
	constexpr uint64 HeaderKey = 0x4D484C4400;
	constexpr uint64 StateKey = HeaderKey + 1;
	constexpr uint64 VelocityKey = HeaderKey + 2;
	constexpr uint64 AccelerationKey = HeaderKey + 3;
	constexpr uint64 MovementConfigKey = HeaderKey + 4;
	constexpr uint64 BrakingConfigKey = HeaderKey + 5;
	constexpr uint64 ErrorKey = HeaderKey + 6;
	constexpr float ArrowDuration = 0.0f;
	constexpr float ArrowSize = 20.0f;
	constexpr float MinimumArrowLength = 50.0f;
	constexpr float MaximumArrowLength = 300.0f;

	void AddMessage(const uint64 Key, const FColor Color, const FString& Message)
	{
		GEngine->AddOnScreenDebugMessage(Key, MessageDuration, Color, Message, false);
	}

	void DrawPlanarArrow(
		const UWorld* World,
		const FVector& Start,
		const FVector& Vector,
		const float LengthScale,
		const FColor Color,
		const FString& Label)
	{
		const float Magnitude = Vector.Size2D();
		if (!World || Magnitude <= KINDA_SMALL_NUMBER)
		{
			return;
		}

		const float ArrowLength = FMath::Clamp(Magnitude * LengthScale, MinimumArrowLength, MaximumArrowLength);
		const FVector End = Start + Vector.GetSafeNormal2D() * ArrowLength;
		DrawDebugDirectionalArrow(
			World,
			Start,
			End,
			ArrowSize,
			Color,
			false,
			ArrowDuration,
			0,
			2.5f);
		DrawDebugString(
			World,
			End + FVector(0.0f, 0.0f, 12.0f),
			Label,
			nullptr,
			Color,
			ArrowDuration,
			true,
			1.0f);
	}
}

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
	if (bIsDebug && IsValid(MH_Character) && MH_Character->IsLocallyControlled())
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
		MHLocomotionDebug::AddMessage(
			MHLocomotionDebug::ErrorKey,
			FColor::Red,
			TEXT("Locomotion Debug: character or movement component is unavailable"));
		return;
	}

	const float PlanarSpeed = VelocityXY.Size();
	const float AccelerationMagnitude = Acceleration.Size();
	const FString GaitName = UEnum::GetValueAsString(GaitType);
	const FString MovementModeName = MH_MovementComponent->GetMovementName();
	const float CapsuleHalfHeight = MH_Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector GroundLocation = MH_Character->GetActorLocation() - FVector(0.0f, 0.0f, CapsuleHalfHeight - 6.0f);
	const FVector PlanarAcceleration(Acceleration.X, Acceleration.Y, 0.0f);

	MHLocomotionDebug::DrawPlanarArrow(
		MH_Character->GetWorld(),
		GroundLocation + FVector(0.0f, 0.0f, 8.0f),
		VelocityXY,
		0.15f,
		FColor::Cyan,
		FString::Printf(TEXT("Velocity: %.1f cm/s"), PlanarSpeed));
	MHLocomotionDebug::DrawPlanarArrow(
		MH_Character->GetWorld(),
		GroundLocation + FVector(0.0f, 0.0f, 24.0f),
		PlanarAcceleration,
		0.08f,
		FColor(255, 165, 0),
		FString::Printf(TEXT("Acceleration: %.1f cm/s^2"), PlanarAcceleration.Size2D()));

	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::HeaderKey,
		FColor::White,
		FString::Printf(TEXT("Locomotion [%s]"), *GetNameSafe(MH_Character.Get())));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::StateKey,
		FColor::Yellow,
		FString::Printf(
			TEXT("State | Gait: %s | Movement Mode: %s | Grounded: %s | Falling: %s | Crouching: %s"),
			*GaitName,
			*MovementModeName,
			MH_MovementComponent->IsMovingOnGround() ? TEXT("true") : TEXT("false"),
			MH_MovementComponent->IsFalling() ? TEXT("true") : TEXT("false"),
			MH_MovementComponent->IsCrouching() ? TEXT("true") : TEXT("false")));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::VelocityKey,
		FColor::Cyan,
		FString::Printf(
			TEXT("Velocity | XYZ: %s | XY: %s | Planar Speed: %.1f"),
			*VelocityXYZ.ToCompactString(),
			*VelocityXY.ToCompactString(),
			PlanarSpeed));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::AccelerationKey,
		FColor(255, 165, 0),
		FString::Printf(TEXT("Acceleration | Value: %s | Magnitude: %.1f"), *Acceleration.ToCompactString(), AccelerationMagnitude));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::MovementConfigKey,
		FColor::Green,
		FString::Printf(
			TEXT("Movement Config | Max Speed: %.1f | Max Acceleration: %.1f | Braking Deceleration: %.1f"),
			MH_MovementComponent->MaxWalkSpeed,
			MH_MovementComponent->MaxAcceleration,
			MH_MovementComponent->BrakingDecelerationWalking));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::BrakingConfigKey,
		FColor(180, 130, 255),
		FString::Printf(
			TEXT("Braking Config | Friction: %.2f | Friction Factor: %.2f | Separate Friction: %s"),
			MH_MovementComponent->BrakingFriction,
			MH_MovementComponent->BrakingFrictionFactor,
			MH_MovementComponent->bUseSeparateBrakingFriction ? TEXT("true") : TEXT("false")));
}
