// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_CharacterAnimInstance.h"

#include "AnimCharacterMovementLibrary.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/MH_CharacterMovementComponent.h"
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
	constexpr uint64 DirectionKey = HeaderKey + 7;
	constexpr uint64 CrouchInputKey = HeaderKey + 8;
	constexpr float ArrowDuration = 0.0f;
	constexpr float ArrowSize = 20.0f;
	constexpr float MinimumArrowLength = 50.0f;
	constexpr float MaximumArrowLength = 300.0f;
	constexpr float PredictedStopPointSize = 16.0f;

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

	void DrawPredictedStopLocation(
		const UWorld* World,
		const FVector& CurrentGroundLocation,
		const FVector& StopOffset,
		const FColor Color,
		const FString& Label)
	{
		if (!World)
		{
			return;
		}

		// CharacterMovementComponent::Velocity 和预测结果均使用世界空间。
		const FVector StopLocation = CurrentGroundLocation + FVector(StopOffset.X, StopOffset.Y, 0.0f);

		DrawDebugLine(World, CurrentGroundLocation, StopLocation, Color, false, ArrowDuration, 0, 1.5f);
		DrawDebugPoint(World, StopLocation, PredictedStopPointSize, Color, false, ArrowDuration, 0);
		DrawDebugString(
			World,
			StopLocation + FVector(0.0f, 0.0f, 18.0f),
			Label,
			nullptr,
			Color,
			ArrowDuration,
			true,
			1.0f);
	}
}

namespace
{
	EMovementDirection GetAccelerationDirection(
		const FVector& Acceleration,
		const FVector& CharacterForward,
		const FVector& CharacterRight)
	{
		const FVector AccelerationDirection = Acceleration.GetSafeNormal2D();
		if (AccelerationDirection.IsNearlyZero() || CharacterForward.IsNearlyZero())
		{
			return EMovementDirection::None;
		}

		const float ForwardDot = FMath::Clamp(
			FVector::DotProduct(CharacterForward, AccelerationDirection), -1.0f, 1.0f);
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(ForwardDot));
		if (Angle <= 45.0f)
		{
			return EMovementDirection::Forward;
		}
		if (Angle >= 135.0f)
		{
			return EMovementDirection::Backward;
		}

		return FVector::DotProduct(CharacterRight, AccelerationDirection) >= 0.0f
			? EMovementDirection::Right
			: EMovementDirection::Left;
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

	if (MH_Character)
	{
		CharacterForward2D = MH_Character->GetActorForwardVector().GetSafeNormal2D();
		CharacterRight2D = MH_Character->GetActorRightVector().GetSafeNormal2D();
	}
	else
	{
		CharacterForward2D = FVector::ForwardVector;
		CharacterRight2D = FVector::RightVector;
	}

	if (MH_Character && MH_MovementComponent)
	{
		bIsPlayingRootMotion = MH_Character->IsPlayingRootMotion();
		bCanPerformLedgeJump = false;
		bIsSliding = false;
		if (const AMH_BasePlayerCharacter* PlayerCharacter = Cast<AMH_BasePlayerCharacter>(MH_Character.Get()))
		{
			if (const UMH_CharacterMovementComponent* PlayerMovement = PlayerCharacter->GetMHCharacterMovementComponent())
			{
				bCanPerformLedgeJump = PlayerMovement->CanPerformLedgeJump();
			}
			bIsSliding = PlayerCharacter->IsSliding();
		}
		const bool bHasMovementInput = !MH_MovementComponent->GetCurrentAcceleration().IsNearlyZero(1.0f);
		const bool bHasStopped = MH_MovementComponent->Velocity.SizeSquared2D() <= FMath::Square(1.0f);

		// 根运动产生的速度完全稳定前，不让它驱动移动状态机。
		if (bIsPlayingRootMotion)
		{
			bIsLocomotionVelocitySuppressed = true;
		}
		else if (bHasMovementInput || bHasStopped)
		{
			bIsLocomotionVelocitySuppressed = false;
		}
	}
	else
	{
		bIsPlayingRootMotion = false;
		bIsLocomotionVelocitySuppressed = false;
		bCanPerformLedgeJump = false;
		bIsSliding = false;
	}
	
	if (LastGaitType != GaitType)
	{
		bGaitChanged = true;
	}else
	{
		bGaitChanged = false;
	}
	LastGaitType = GaitType;

	if (bIsDebug && IsValid(MH_Character) && MH_Character->IsLocallyControlled())
	{
		Debug();
	}
}


void UMH_CharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	DeltaDistance = 0.0f;
	bAccelerationChange = false;
	AccelerationVelocityAngle = 0.0f;
	AccelerationVelocityDirection = EMovementDirection::None;
	if (!MH_MovementComponent || !MH_Character || DeltaSeconds <= 0.0f)
	{
		LastAcceleration2D = Acceleration2D;
		Acceleration2D = FVector::ZeroVector;
		AccelerationDirection = EMovementDirection::None;
		bIsAccelerating = false;
		return;
	}
	
	VelocityXYZ = MH_MovementComponent->Velocity;
	DeltaDistance = VelocityXYZ.Size2D() * DeltaSeconds;
	VelocityXY = bIsLocomotionVelocitySuppressed
		? FVector::ZeroVector
		: FVector(VelocityXYZ.X, VelocityXYZ.Y, 0.0f);
	
	const FVector PreviousAcceleration2D = Acceleration2D;
	LastAcceleration2D = PreviousAcceleration2D;
	Acceleration2D = MH_MovementComponent->GetCurrentAcceleration();
	Acceleration2D = FVector(Acceleration2D.X,Acceleration2D.Y,0.0f);
	bIsAccelerating = Acceleration2D.SizeSquared() > 0.0001f;

	AccelerationDirectionAngle = 0.0f;
	const FVector AccelerationVectorDirection = Acceleration2D.GetSafeNormal2D();
	AccelerationDirection = GetAccelerationDirection(Acceleration2D, CharacterForward2D, CharacterRight2D);
	if (!AccelerationVectorDirection.IsNearlyZero() && !CharacterForward2D.IsNearlyZero())
	{
		const float ForwardDot = FMath::Clamp(FVector::DotProduct(CharacterForward2D, AccelerationVectorDirection), -1.0f, 1.0f);
		AccelerationDirectionAngle = FMath::RadiansToDegrees(FMath::Acos(ForwardDot));
	}

	const FVector VelocityDirection = FVector(VelocityXYZ.X, VelocityXYZ.Y, 0.0f).GetSafeNormal2D();
	if (!VelocityDirection.IsNearlyZero() && !AccelerationVectorDirection.IsNearlyZero())
	{
		const float VelocityDot = FMath::Clamp(
			FVector::DotProduct(VelocityDirection, AccelerationVectorDirection), -1.0f, 1.0f);
		AccelerationVelocityAngle = FMath::RadiansToDegrees(FMath::Acos(VelocityDot));

		if (AccelerationVelocityAngle <= 45.0f)
		{
			AccelerationVelocityDirection = EMovementDirection::Forward;
		}
		else if (AccelerationVelocityAngle >= 135.0f)
		{
			AccelerationVelocityDirection = EMovementDirection::Backward;
		}
		else
		{
			const FVector VelocityRight = FVector(-VelocityDirection.Y, VelocityDirection.X, 0.0f);
			AccelerationVelocityDirection = FVector::DotProduct(VelocityRight, AccelerationVectorDirection) >= 0.0f
				? EMovementDirection::Right
				: EMovementDirection::Left;
		}
	}

	const EMovementDirection PreviousDirection = GetAccelerationDirection(
		PreviousAcceleration2D, CharacterForward2D, CharacterRight2D);
	bAccelerationChange = PreviousDirection != EMovementDirection::None
		&& AccelerationDirection != EMovementDirection::None
		&& PreviousDirection != AccelerationDirection;
	//在下一次改变之前都保存需要前往的方向
	ChangeDirectionTo = bAccelerationChange ? AccelerationDirection : ChangeDirectionTo;

	PredictStopDistance = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(VelocityXY,
		MH_MovementComponent->bUseSeparateBrakingFriction,
		MH_MovementComponent->BrakingFriction,
		MH_MovementComponent->GroundFriction,
		MH_MovementComponent->BrakingFrictionFactor,
		MH_MovementComponent->BrakingDecelerationWalking);
	
	PredictPivotDistance = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
		MH_MovementComponent->GetCurrentAcceleration(),
		MH_MovementComponent->Velocity,
		MH_MovementComponent->GroundFriction
	);
	
	if (MH_MovementComponent->GetCurrentAcceleration().Z > 0.0f && MH_MovementComponent->IsFalling())
	{
		bIsJumping = true;
		bIsFalling = true;
	}else if ( MH_MovementComponent->IsFalling() && MH_MovementComponent->GetCurrentAcceleration().Z <= 0.0f)
	{
		bIsFalling = true;
		bIsFalling = true;
	}

}

void UMH_CharacterAnimInstance::SetGaitType(const FGaitType InStateType)
{
	LastGaitType = GaitType;
	GaitType = InStateType;
}

void UMH_CharacterAnimInstance::SetMoveState(const EMoveState InMoveState)
{
	MoveState = InMoveState;
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

	const float RawPlanarSpeed = VelocityXYZ.Size2D();
	const float LocomotionPlanarSpeed = VelocityXY.Size2D();
	const float AccelerationMagnitude = Acceleration2D.Size();
	const FString GaitName = UEnum::GetValueAsString(GaitType);
	const FString MoveStateName = UEnum::GetValueAsString(MoveState);
	const FString MovementModeName = MH_MovementComponent->GetMovementName();
	const AMH_BasePlayerCharacter* PlayerCharacter = Cast<AMH_BasePlayerCharacter>(MH_Character.Get());
	const float CapsuleHalfHeight = MH_Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector GroundLocation = MH_Character->GetActorLocation() - FVector(0.0f, 0.0f, CapsuleHalfHeight - 6.0f);
	const FVector PlanarAcceleration(Acceleration2D.X, Acceleration2D.Y, 0.0f);

	MHLocomotionDebug::DrawPlanarArrow(
		MH_Character->GetWorld(),
		GroundLocation + FVector(0.0f, 0.0f, 8.0f),
		FVector(VelocityXYZ.X, VelocityXYZ.Y, 0.0f),
		0.15f,
		FColor::Cyan,
		FString::Printf(TEXT("Velocity: %.1f cm/s"), RawPlanarSpeed));
	MHLocomotionDebug::DrawPlanarArrow(
		MH_Character->GetWorld(),
		GroundLocation + FVector(0.0f, 0.0f, 24.0f),
		PlanarAcceleration,
		0.08f,
		FColor(255, 165, 0),
		FString::Printf(TEXT("Acceleration: %.1f cm/s^2"), PlanarAcceleration.Size2D()));
	MHLocomotionDebug::DrawPredictedStopLocation(
		MH_Character->GetWorld(),
		GroundLocation,
		PredictStopDistance,
		FColor::Magenta,
		FString::Printf(
			TEXT("Predicted Stop: %.1f cm | World Offset: %s"),
			PredictStopDistance.Size2D(),
			*PredictStopDistance.ToCompactString()));

	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::HeaderKey,
		FColor::White,
		FString::Printf(TEXT("Locomotion [%s]"), *GetNameSafe(MH_Character.Get())));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::StateKey,
		FColor::Yellow,
		FString::Printf(
			TEXT("State | Gait: %s | Move State: %s | Movement Mode: %s | Root Motion: %s | Suppressed: %s | Grounded: %s | Falling: %s | Crouching: %s"),
			*GaitName,
			*MoveStateName,
			*MovementModeName,
			bIsPlayingRootMotion ? TEXT("true") : TEXT("false"),
			bIsLocomotionVelocitySuppressed ? TEXT("true") : TEXT("false"),
			MH_MovementComponent->IsMovingOnGround() ? TEXT("true") : TEXT("false"),
			MH_MovementComponent->IsFalling() ? TEXT("true") : TEXT("false"),
			MH_MovementComponent->IsCrouching() ? TEXT("true") : TEXT("false")));
	if (PlayerCharacter)
	{
		const ECrouchInputAction PredictedAction = PlayerCharacter->GetPredictedCrouchInputAction();
		const ECrouchInputReason PredictedReason = PlayerCharacter->GetPredictedCrouchInputReason();
		const FString CrouchActionName = UEnum::GetValueAsString(PredictedAction);
		const FString CrouchReasonName = UEnum::GetValueAsString(PredictedReason);
		const FColor CrouchActionColor = PredictedAction == ECrouchInputAction::Jump
			? FColor::Red
			: PredictedAction == ECrouchInputAction::Crouch
				? FColor::Green
				: FColor::Yellow;

		MHLocomotionDebug::AddMessage(
			MHLocomotionDebug::CrouchInputKey,
			CrouchActionColor,
			FString::Printf(
				TEXT("Crouch Input | Action: %s | Reason: %s | Slope: %.1f deg | Slide Speed: %.1f"),
				* CrouchActionName,
				* CrouchReasonName,
				PlayerCharacter->GetGroundSlopeAngle(),
				PlayerCharacter->GetSlideSpeed()));
	}
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::VelocityKey,
		FColor::Cyan,
		FString::Printf(
			TEXT("Velocity | Raw XYZ: %s | Locomotion XY: %s | Raw Speed: %.1f | Locomotion Speed: %.1f"),
			*VelocityXYZ.ToCompactString(),
			*VelocityXY.ToCompactString(),
			RawPlanarSpeed,
			LocomotionPlanarSpeed));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::AccelerationKey,
		FColor(255, 165, 0),
		FString::Printf(TEXT("Acceleration | Value: %s | Magnitude: %.1f"), *Acceleration2D.ToCompactString(), AccelerationMagnitude));
	MHLocomotionDebug::AddMessage(
		MHLocomotionDebug::DirectionKey,
		FColor::Orange,
		FString::Printf(
			TEXT("Acceleration/Character | Angle: %.1f deg | Direction: %s"),
			AccelerationDirectionAngle,
			*UEnum::GetValueAsString(AccelerationDirection)));
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
