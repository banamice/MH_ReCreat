// Copyright Epic Games, Inc. All Rights Reserved.

#include "Component/MH_CharacterMovementComponent.h"

#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"

UMH_CharacterMovementComponent::UMH_CharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMH_CharacterMovementComponent::OnMovementUpdated(const float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (IsFalling())
	{
		if (AMH_BasePlayerCharacter* PlayerCharacter = Cast<AMH_BasePlayerCharacter>(GetCharacterOwner());
			PlayerCharacter && (PlayerCharacter->GetBaseGaitType() != FGaitType::Walk
				|| PlayerCharacter->MoveState != EMoveState::None
				|| IsCrouching()))
		{
			PlayerCharacter->ResetMovementStateForJump();
		}
	}
	UpdateSurfaceState();
}

void UMH_CharacterMovementComponent::RefreshSurfaceState()
{
	UpdateSurfaceState();
}

void UMH_CharacterMovementComponent::SetAirborneControlEnabled(const bool bEnabled)
{
	bAirborneMovementEnabled = bEnabled;
	bAirborneRotationEnabled = bEnabled;
	if (MovementMode == MOVE_Falling)
	{
		ApplyAirborneControlState();
	}
}

void UMH_CharacterMovementComponent::SetAirborneMovementEnabled(const bool bEnabled)
{
	bAirborneMovementEnabled = bEnabled;
	if (MovementMode == MOVE_Falling)
	{
		ApplyAirborneControlState();
	}
}

void UMH_CharacterMovementComponent::SetAirborneRotationEnabled(const bool bEnabled)
{
	bAirborneRotationEnabled = bEnabled;
	if (MovementMode == MOVE_Falling)
	{
		ApplyAirborneControlState();
	}
}

void UMH_CharacterMovementComponent::OnMovementModeChanged(
	const EMovementMode PreviousMovementMode,
	const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Falling && PreviousMovementMode != MOVE_Falling)
	{
		if (ACharacter* Character = GetCharacterOwner())
		{
			CachedAirControl = AirControl;
			CachedOrientRotationToMovement = bOrientRotationToMovement;
			CachedUseControllerRotationYaw = Character->bUseControllerRotationYaw;
			bAirborneSettingsCached = true;
		}

		// 默认锁定空中控制；如果能力已提前开启，则保留该显式请求。
		ApplyAirborneControlState();
	}
	else if (MovementMode != MOVE_Falling && PreviousMovementMode == MOVE_Falling)
	{
		if (ACharacter* Character = GetCharacterOwner(); bAirborneSettingsCached && Character)
		{
			AirControl = CachedAirControl;
			bOrientRotationToMovement = CachedOrientRotationToMovement;
			Character->bUseControllerRotationYaw = CachedUseControllerRotationYaw;
		}

		bAirborneSettingsCached = false;
		bAirborneMovementEnabled = false;
		bAirborneRotationEnabled = false;
	}
}

void UMH_CharacterMovementComponent::ApplyAirborneControlState()
{
	if (MovementMode != MOVE_Falling)
	{
		return;
	}

	ACharacter* Character = GetCharacterOwner();
	if (!Character)
	{
		return;
	}

	// 禁用移动时保留起跳已有速度，只忽略新的空中输入。
	if (bAirborneMovementEnabled)
	{
		AirControl = bAirborneSettingsCached ? CachedAirControl : AirControl;
	}
	else
	{
		AirControl = 0.0f;
	}

	if (bAirborneRotationEnabled)
	{
		bOrientRotationToMovement = bAirborneSettingsCached
			? CachedOrientRotationToMovement
			: true;
		Character->bUseControllerRotationYaw = bAirborneSettingsCached
			? CachedUseControllerRotationYaw
			: false;
	}
	else
	{
		bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = false;
	}
}

void UMH_CharacterMovementComponent::UpdateSurfaceState()
{
	ResetSurfaceState();
	const float FloorContactTolerance = FMath::Max(2.0f, MaxStepHeight + 2.0f);
	if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist <= FloorContactTolerance)
	{
		const FVector FloorNormal = CurrentFloor.HitResult.Normal.GetSafeNormal();
		if (!FloorNormal.IsNearlyZero())
		{
			const float NormalUpDot = FMath::Clamp(FVector::DotProduct(FloorNormal, FVector::UpVector), -1.0f, 1.0f);
			GroundSlopeAngle = FMath::RadiansToDegrees(FMath::Acos(NormalUpDot));
			SlideDirection = FVector::VectorPlaneProject(-FVector::UpVector, FloorNormal).GetSafeNormal();
			if (GroundSlopeAngle >= SlideStartAngle && !SlideDirection.IsNearlyZero())
			{
				bCanSlide = true;
				SlideSpeed = FMath::Max(0.0f, FVector::DotProduct(Velocity, SlideDirection));
				bIsSliding = SlideSpeed >= SlideStartSpeed;
			}
		}
	}
	UpdateLedgeState();
}

void UMH_CharacterMovementComponent::ResetSurfaceState()
{
	bCanSlide = false;
	bIsSliding = false;
	GroundSlopeAngle = 0.0f;
	SlideSpeed = 0.0f;
	SlideDirection = FVector::ZeroVector;
	ResetLedgeState();
}

void UMH_CharacterMovementComponent::UpdateLedgeState()
{
	ResetLedgeState();
	const ACharacter* Character = GetCharacterOwner();
	const UCapsuleComponent* CharacterCapsule = Character ? Character->GetCapsuleComponent() : nullptr;
	const UWorld* World = GetWorld();
	if (!Character || !CharacterCapsule || !World || IsFalling())
	{
		return;
	}
	const float FloorContactTolerance = FMath::Max(2.0f, MaxStepHeight + 2.0f);
	if (!CurrentFloor.bBlockingHit || CurrentFloor.FloorDist > FloorContactTolerance)
	{
		return;
	}
	const FVector ForwardDirection = Character->GetActorForwardVector().GetSafeNormal2D();
	if (ForwardDirection.IsNearlyZero())
	{
		return;
	}
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MHForwardLedge), false, Character);
	QueryParams.AddIgnoredActor(Character);
	const FVector WallStart = Character->GetActorLocation();
	const FVector WallEnd = WallStart + ForwardDirection * LedgeProbeDistance;
	if (World->LineTraceTestByChannel(WallStart, WallEnd, LedgeTraceChannel, QueryParams))
	{
		return;
	}
	const float CapsuleHalfHeight = CharacterCapsule->GetScaledCapsuleHalfHeight();
	const FVector CharacterBase = Character->GetActorLocation() - FVector::UpVector * CapsuleHalfHeight;
	const FVector ProbeLocation = CharacterBase + ForwardDirection * LedgeProbeDistance;
	float TraceEndZ = World->GetWorldSettings() ? World->GetWorldSettings()->KillZ : ProbeLocation.Z - 100000.0f;
	if (TraceEndZ >= ProbeLocation.Z)
	{
		TraceEndZ = ProbeLocation.Z - 100000.0f;
	}
	FHitResult LedgeHit;
	const FVector TraceStart(ProbeLocation.X, ProbeLocation.Y, ProbeLocation.Z + CapsuleHalfHeight * 2.0f);
	const FVector TraceEnd(ProbeLocation.X, ProbeLocation.Y, TraceEndZ);
	if (!World->LineTraceSingleByChannel(LedgeHit, TraceStart, TraceEnd, LedgeTraceChannel, QueryParams))
	{
		return;
	}
	LedgeDropHeight = CharacterBase.Z - LedgeHit.ImpactPoint.Z;
	bCanPerformLedgeJump = LedgeDropHeight >= CapsuleHalfHeight * 2.0f;
}

void UMH_CharacterMovementComponent::ResetLedgeState()
{
	bCanPerformLedgeJump = false;
	LedgeDropHeight = 0.0f;
}
