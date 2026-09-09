// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Player/MH_GA_Crouch.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BPFuncLib/MH_BluePrintFuncLib.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

namespace
{
	constexpr float HeightSearchIterations = 8.0f;
	constexpr float TraceEpsilon = 2.0f;
}

/** 检测结果只在本次能力激活期间使用，不参与网络复制。 */
struct FClimbDetectionResult
{
	FHitResult WallHit;
	FVector WallNormal = FVector::ZeroVector;
	FVector ApproachLocation = FVector::ZeroVector;
	FVector MantleLocation = FVector::ZeroVector;
	FVector LandingLocation = FVector::ZeroVector;
	FRotator FacingRotation = FRotator::ZeroRotator;
	float Height = 0.0f;
	float Depth = 0.0f;
};

UMH_GA_Crouch::UMH_GA_Crouch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bRetriggerInstancedAbility = false;
	bReplicateInputDirectly = true;
}

void UMH_GA_Crouch::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogMH, Warning, TEXT("%s: Crouch ability has no player character"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
		!MovementComponent || MovementComponent->IsFalling())
	{
		// 只有地面状态才允许触发攀爬或下蹲。
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 已经蹲下时本次输入仍然保持原有的“起身”语义，不尝试攀爬。
	if (Character->GetBaseGaitType() != FGaitType::Crouch && !Character->IsCrouched() && TryStartClimb())
	{
		return;
	}

	// 保留项目原有的滑坡/悬崖情境跳跃逻辑。
	if (Character->TryPerformContextualJump())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (Character->GetBaseGaitType() == FGaitType::Crouch || Character->IsCrouched())
	{
		Character->ResetMovementStateForJump();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!Character->CanCrouch())
	{
		UE_LOG(LogMH, Warning, TEXT("%s: Crouch request rejected"), *Character->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Character->Crouch();
	if (!Character->SetBaseGaitType(FGaitType::Crouch))
	{
		Character->UnCrouch();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UMH_GA_Crouch::TryStartClimb()
{
	FClimbDetectionResult Detection;
	if (!DetectClimbSurface(Detection))
	{
		return false;
	}

	const EMH_ClimbVariant Variant = ClassifyClimbSurface(Detection);
	UAnimMontage* ClimbMontage = GetMontageForVariant(Variant);
	if (!IsValid(ClimbMontage))
	{
		UE_LOG(LogMH, Verbose, TEXT("%s: no montage configured for climb variant %d"), *GetName(), static_cast<uint8>(Variant));
		return false;
	}

	AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	UMotionWarpingComponent* MotionWarpingComponent = Character
		? Character->FindComponentByClass<UMotionWarpingComponent>()
		: nullptr;
	if (!IsValid(MotionWarpingComponent))
	{
		UE_LOG(LogMH, Warning, TEXT("%s: player has no MotionWarpingComponent"), *GetName());
		return false;
	}

	ConfigureWarpTargets(Detection);
	ActiveMotionWarpingComponent = MotionWarpingComponent;
	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		// 攀爬根运动包含明显的 Z 轴位移。Flying 模式不会被 Walking 的地面约束截断，
		// 客户端预测和服务器执行都会切换，结束时再恢复原移动模式。
		PreviousMovementMode = MovementComponent->MovementMode;
		PreviousCustomMovementMode = MovementComponent->CustomMovementMode;
		bMovementModeChanged = true;
		MovementComponent->SetMovementMode(MOVE_Flying);
	}
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ClimbMontage, 1.0f, NAME_None, true);
	if (!MontageTask)
	{
		ClearWarpTargets();
		RestoreMovementMode();
		return false;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnClimbMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnClimbMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnClimbMontageCancelled);
	MontageTask->ReadyForActivation();

	return true;
}

bool UMH_GA_Crouch::DetectClimbSurface(FClimbDetectionResult& OutResult)
{
	const AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	const UCapsuleComponent* Capsule = Character ? Character->GetCapsuleComponent() : nullptr;
	const UCharacterMovementComponent* Movement = Character ? Character->GetCharacterMovement() : nullptr;
	UWorld* World = Character ? Character->GetWorld() : nullptr;
	if (!Character || !Capsule || !Movement || !World)
	{
		return false;
	}
	const FVector Forward = Character->GetActorForwardVector().GetSafeNormal2D();
	if (Forward.IsNearlyZero())
	{
		return false;
	}
	const FVector Up = FVector::UpVector;
	const FVector ActorLocation = Character->GetActorLocation();
	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const float FeetZ = ActorLocation.Z - CapsuleHalfHeight;
	const float MinimumHeight = FMath::Max(
		MinimumObstacleHeight,
		bUseCharacterMaxStepHeightAsMinimum ? Movement->MaxStepHeight + 1.0f : 0.0f);
	if (MaximumObstacleHeight <= MinimumHeight)
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MHClimbWall), false, Character);
	FHitResult WallHit;
	const FVector WallTraceStart = ActorLocation + Up * (MinimumHeight + 2.0f - CapsuleHalfHeight);
	const FVector WallTraceEnd = WallTraceStart + Forward * ForwardProbeDistance;
	if (!World->LineTraceSingleByChannel(WallHit, WallTraceStart, WallTraceEnd, ClimbTraceChannel, QueryParams)
		|| !WallHit.bBlockingHit || !WallHit.Component.IsValid()
		|| FMath::Abs(WallHit.ImpactNormal.Z) > 0.25f
		|| FVector::DotProduct(WallHit.ImpactNormal, -Forward) < 0.5f)
	{
		return false;
	}
	// 通过二分查找“水平射线刚好不再命中”的位置，得到墙体高度。
	auto IsBlockingAtHeight = [&](const float Height)
	{
		FHitResult HeightHit;
		const FVector Start = FVector(ActorLocation.X, ActorLocation.Y, FeetZ + Height);
		return World->LineTraceSingleByChannel(HeightHit, Start, Start + Forward * ForwardProbeDistance,
			ClimbTraceChannel, QueryParams) && HeightHit.bBlockingHit;
	};

	float LowHeight = MinimumHeight;
	float HighHeight = MaximumObstacleHeight;
	const bool bBlockedAtMinimumHeight = IsBlockingAtHeight(MinimumHeight);
	if (!bBlockedAtMinimumHeight)
	{
		return false;
	}
	const bool bBlockedAtMaximumHeight = IsBlockingAtHeight(HighHeight);
	if (bBlockedAtMaximumHeight)
	{
		return false;
	}
	for (int32 Index = 0; Index < static_cast<int32>(HeightSearchIterations); ++Index)
	{
		const float MidHeight = (LowHeight + HighHeight) * 0.5f;
		const bool bBlocked = IsBlockingAtHeight(MidHeight);
		if (bBlocked)
		{
			LowHeight = MidHeight;
		}
		else
		{
			HighHeight = MidHeight;
		}
	}

	OutResult.Height = LowHeight;
	if (OutResult.Height < MinimumHeight || OutResult.Height > MaximumObstacleHeight)
	{
		return false;
	}
	OutResult.WallHit = WallHit;
	OutResult.WallNormal = WallHit.ImpactNormal.GetSafeNormal2D();
	OutResult.FacingRotation = (-OutResult.WallNormal).Rotation();

	// 从墙体后方反向检测前后两个面，估算墙体厚度；没有出口则按最大深度处理。
	const float FrontDistance = FMath::Max(0.0f, FVector::DotProduct(WallHit.ImpactPoint - ActorLocation, Forward));
	const float TraceHeight = FMath::Clamp(OutResult.Height * 0.5f, MinimumHeight + 2.0f, OutResult.Height - 1.0f);
	const FVector DepthStart = FVector(ActorLocation.X, ActorLocation.Y, FeetZ + TraceHeight)
		+ Forward * (FrontDistance + MaximumObstacleDepth + TraceEpsilon);
	const FVector DepthEnd = FVector(ActorLocation.X, ActorLocation.Y, FeetZ + TraceHeight)
		+ Forward * FMath::Max(0.0f, FrontDistance - TraceEpsilon);
	TArray<FHitResult> DepthHits;
	World->LineTraceMultiByChannel(DepthHits, DepthStart, DepthEnd, ClimbTraceChannel, QueryParams);
	float FarFaceDistance = -1.0f;
	float NearFaceDistance = -1.0f;
	for (const FHitResult& Hit : DepthHits)
	{
		if (Hit.Component == WallHit.Component)
		{
			const float Distance = FVector::DotProduct(Hit.ImpactPoint - ActorLocation, Forward);
			if (FarFaceDistance < 0.0f)
			{
				FarFaceDistance = Distance;
			}
			NearFaceDistance = Distance;
		}
	}
	OutResult.Depth = (FarFaceDistance >= 0.0f && NearFaceDistance >= 0.0f)
		? FMath::Max(0.0f, FarFaceDistance - NearFaceDistance)
		: MaximumObstacleDepth;

	const FVector FrontPoint = WallHit.ImpactPoint + OutResult.WallNormal * (CapsuleRadius + WarpTargetClearance);
	OutResult.ApproachLocation = FVector(FrontPoint.X, FrontPoint.Y, ActorLocation.Z);
	// 墙顶目标位于墙体厚度中线，而不是墙体前表面；否则低墙第二阶段只会上升，
	// 角色仍停在墙前，第三阶段开始时容易从墙顶边缘掉落。
	const FVector WallTopCenter = FrontPoint + Forward * (OutResult.Depth * 0.5f);
	OutResult.MantleLocation = FVector(WallTopCenter.X, WallTopCenter.Y, FeetZ + OutResult.Height + CapsuleHalfHeight);

	if (OutResult.Depth > MaximumObstacleDepth)
	{
		return false;
	}

	// 第三阶段统一落在墙顶前方一个身位；是否因墙体过薄而下落由角色根运动和碰撞处理。
	OutResult.LandingLocation = OutResult.MantleLocation + Forward * (CapsuleRadius * 2.0f);
	if (bDrawClimbDebug)
	{
		// 仅绘制三个阶段目标点，不绘制检测射线。
		DrawDebugSphere(World, OutResult.ApproachLocation, 12.0f, 16, FColor::Blue, false, 2.0f, 0, 2.0f);
		DrawDebugSphere(World, OutResult.MantleLocation, 12.0f, 16, FColor::Yellow, false, 2.0f, 0, 2.0f);
		DrawDebugSphere(World, OutResult.LandingLocation, 12.0f, 16, FColor::Green, false, 2.0f, 0, 2.0f);
	}
	return true;
}

EMH_ClimbVariant UMH_GA_Crouch::ClassifyClimbSurface(const FClimbDetectionResult& Detection) const
{
	const bool bHighWall = Detection.Height >= HighWallHeightThreshold;
	const bool bThinWall = Detection.Depth <= ThinWallDepthThreshold;
	if (bHighWall)
	{
		return bThinWall ? EMH_ClimbVariant::HighThinWall : EMH_ClimbVariant::HighThickWall;
	}
	return bThinWall ? EMH_ClimbVariant::LowThinWall : EMH_ClimbVariant::LowThickWall;
}

UAnimMontage* UMH_GA_Crouch::GetMontageForVariant(const EMH_ClimbVariant Variant) const
{
	switch (Variant)
	{
	case EMH_ClimbVariant::LowThinWall: return LowThinWallMontage;
	case EMH_ClimbVariant::LowThickWall: return LowThickWallMontage;
	case EMH_ClimbVariant::HighThinWall: return HighThinWallMontage;
	case EMH_ClimbVariant::HighThickWall: return HighThickWallMontage;
	default: return nullptr;
	}
}

void UMH_GA_Crouch::ConfigureWarpTargets(const FClimbDetectionResult& Detection)
{
	if (UMotionWarpingComponent* MotionWarpingComponent = GetPlayerCharacter()
		? GetPlayerCharacter()->FindComponentByClass<UMotionWarpingComponent>() : nullptr)
	{
		// LocalPredicted 能力会在客户端和服务器各执行一次检测；MotionWarpingComponent
		// 会将目标同步给模拟端，预测端先使用本地结果，蒙太奇根运动由 GAS 任务同步。
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
			ApproachWarpTargetName, Detection.ApproachLocation, Detection.FacingRotation);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
			MantleWarpTargetName, Detection.MantleLocation, Detection.FacingRotation);

		// 四种蒙太奇的第三阶段目标统一为墙顶前方一个身位。
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
			LandingWarpTargetName, Detection.LandingLocation, Detection.FacingRotation);
	}
}

void UMH_GA_Crouch::ClearWarpTargets()
{
	if (UMotionWarpingComponent* MotionWarpingComponent = ActiveMotionWarpingComponent.Get())
	{
		TArray<FName> Names{ApproachWarpTargetName, MantleWarpTargetName, LandingWarpTargetName};
		MotionWarpingComponent->RemoveWarpTargets(Names);
	}
	ActiveMotionWarpingComponent.Reset();
}

void UMH_GA_Crouch::OnClimbMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMH_GA_Crouch::OnClimbMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMH_GA_Crouch::RestoreMovementMode()
{
	if (!bMovementModeChanged)
	{
		return;
	}
	if (AMH_BasePlayerCharacter* Character = GetPlayerCharacter())
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(PreviousMovementMode, PreviousCustomMovementMode);
			// 本能力只允许在地面激活；如果原模式无效，确保角色回到可行走状态。
			if (MovementComponent->MovementMode == MOVE_None)
			{
				MovementComponent->SetMovementMode(MOVE_Walking);
			}
		}
	}
	bMovementModeChanged = false;
}

void UMH_GA_Crouch::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const bool bReplicateEndAbility,
	const bool bWasCancelled)
{
	ClearWarpTargets();
	RestoreMovementMode();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
