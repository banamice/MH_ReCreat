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
