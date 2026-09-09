// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_CharacterAnimInstance.h"

#include "AnimCharacterMovementLibrary.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/MH_CharacterMovementComponent.h"
#include "Component/CombatComponent/MH_PlayerCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapon/MH_BaseWeapon.h"

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
	// 角色右手持武器时使用的挂点。FABRIK 节点也以此 Socket 作为目标参考。
	const FName WeaponHoldSocketName(TEXT("RHandWeapon"));

	EMovementDirection GetRelativeMovementDirection(
		const FVector& Acceleration,
		const FVector& ReferenceForward,
		const FVector& ReferenceRight)
	{
		const FVector AccelerationDirection = Acceleration.GetSafeNormal2D();
		if (AccelerationDirection.IsNearlyZero()
			|| ReferenceForward.IsNearlyZero()
			|| ReferenceRight.IsNearlyZero())
		{
			return EMovementDirection::None;
		}

		const float ForwardDot = FMath::Clamp(
			FVector::DotProduct(ReferenceForward, AccelerationDirection), -1.0f, 1.0f);
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(ForwardDot));
		if (Angle <= 45.0f)
		{
			return EMovementDirection::Forward;
		}

		const float RightDot = FVector::DotProduct(ReferenceRight, AccelerationDirection);
		if (Angle >= 135.0f)
		{
			return RightDot > 0.0f
				? EMovementDirection::BackwardRight
				: EMovementDirection::BackwardLeft;
		}

		return RightDot >= 0.0f
			? EMovementDirection::Right
			: EMovementDirection::Left;
	}

	bool IsBackwardDirection(const EMovementDirection Direction)
	{
		return Direction == EMovementDirection::BackwardLeft
			|| Direction == EMovementDirection::BackwardRight;
	}

	bool AreSameMajorMovementDirection(
		const EMovementDirection FirstDirection,
		const EMovementDirection SecondDirection)
	{
		const bool bFirstIsBackward = IsBackwardDirection(FirstDirection);
		const bool bSecondIsBackward = IsBackwardDirection(SecondDirection);
		return bFirstIsBackward || bSecondIsBackward
			? bFirstIsBackward && bSecondIsBackward
			: FirstDirection == SecondDirection;
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

	// 武器附着在右手时，左手目标由武器的 LeftHandIK Socket 提供。
	// 目标由纯局部空间变换组合得到，避免移动时读取世界 Socket 造成附着更新顺序偏差。
	LeftHandIKTransform = FTransform::Identity;
	bLeftHandIKEnabled = false;
	if (const AMH_BasePlayerCharacter* PlayerCharacter = Cast<AMH_BasePlayerCharacter>(MH_Character.Get()))
	{
		const UMH_PlayerCombatComponent* CombatComponent = PlayerCharacter->GetMHCombatComponent();
		const AMH_BaseWeapon* HoldWeapon = CombatComponent ? CombatComponent->GetHoldWeapon() : nullptr;
		const USkeletalMeshComponent* CharacterMesh = PlayerCharacter->GetMesh();
		const bool bWeaponIKReady = CombatComponent
			&& CombatComponent->IsWeaponEquipMontageFinished();
		if (bWeaponIKReady && HoldWeapon && HoldWeapon->HasLeftHandIKSocket() && CharacterMesh)
		{
			const USceneComponent* WeaponRoot = HoldWeapon->GetRootComponent();
			if (WeaponRoot
				&& WeaponRoot->GetAttachParent() == CharacterMesh)
			{
				const FName AttachSocketName = WeaponRoot->GetAttachSocketName();
				if (AttachSocketName == WeaponHoldSocketName)
				{
					// FABRIK 的目标参考是角色 RHandWeapon Socket（Bone Space）。
					// 这里只保存武器 LeftHandIK 相对该挂点的局部偏移，避免在 C++ 中读取动画骨骼姿势。
					// FABRIK 会在动画评估阶段用当前帧的 RHandWeapon Socket 重建目标，移动时不会产生固定偏移。
					const FTransform WeaponSocketLocal = HoldWeapon->GetLeftHandIKComponentTransform();
					LeftHandIKTransform = WeaponSocketLocal
						* WeaponRoot->GetRelativeTransform();
					// Socket 存在且武器已附着到角色网格时，Identity 也可能是合法的局部变换，不能用数值判断开关。
					bLeftHandIKEnabled = true;
				}
			}
		}
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
		bIsJumping = false;
		bIsFalling = false;
		bIsOnAir = false;
		TimeToJumpApex = 0.0f;
		return;
	}
	
	VelocityXYZ = MH_MovementComponent->Velocity;
	const bool bMovementIsFalling = MH_MovementComponent->IsFalling();
	const bool bMovementIsAscending = VelocityXYZ.Z > KINDA_SMALL_NUMBER;
	// 使用实际垂直速度区分上升和下落，不使用 GetCurrentAcceleration().Z。
	bIsOnAir = bMovementIsFalling;
	bIsJumping = bMovementIsFalling && bMovementIsAscending;
	bIsFalling = bMovementIsFalling && !bMovementIsAscending;
	const float GravityMagnitude = FMath::Abs(MH_MovementComponent->GetGravityZ());
	TimeToJumpApex = bIsJumping && GravityMagnitude > KINDA_SMALL_NUMBER
		? FMath::Max(0.0f, VelocityXYZ.Z / GravityMagnitude)
		: 0.0f;
	DeltaDistance = VelocityXYZ.Size2D() * DeltaSeconds;
	/*VelocityXY = bIsLocomotionVelocitySuppressed
		? FVector::ZeroVector
		: FVector(VelocityXYZ.X, VelocityXYZ.Y, 0.0f);*/
	VelocityXY =FVector(VelocityXYZ.X, VelocityXYZ.Y, 0.0f);
	
	const FVector PreviousAcceleration2D = Acceleration2D;
	LastAcceleration2D = PreviousAcceleration2D;
	Acceleration2D = MH_MovementComponent->GetCurrentAcceleration();
	Acceleration2D = FVector(Acceleration2D.X,Acceleration2D.Y,0.0f);
	bIsAccelerating = Acceleration2D.SizeSquared() > 0.0001f;

	AccelerationDirectionAngle = 0.0f;
	const FVector AccelerationVectorDirection = Acceleration2D.GetSafeNormal2D();
	AccelerationDirection = GetRelativeMovementDirection(Acceleration2D, CharacterForward2D, CharacterRight2D);
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

		AccelerationVelocityDirection = GetRelativeMovementDirection(
			Acceleration2D,
			VelocityDirection,
			FVector(-VelocityDirection.Y, VelocityDirection.X, 0.0f));
	}

	const EMovementDirection PreviousDirection = GetRelativeMovementDirection(
		PreviousAcceleration2D, CharacterForward2D, CharacterRight2D);
	bAccelerationChange = PreviousDirection != EMovementDirection::None
		&& AccelerationDirection != EMovementDirection::None
		&& !AreSameMajorMovementDirection(PreviousDirection, AccelerationDirection);
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
