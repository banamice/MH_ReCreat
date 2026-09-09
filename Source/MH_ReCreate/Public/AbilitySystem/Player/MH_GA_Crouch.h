// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MH_PlayerGameplayAbility.h"
#include "MH_GA_Crouch.generated.h"

class UAnimMontage;
class UMotionWarpingComponent;
struct FClimbDetectionResult;

UENUM(BlueprintType)
enum class EMH_ClimbVariant : uint8
{
	LowThinWall,
	LowThickWall,
	HighThinWall,
	HighThickWall
};

// 切换 Walk/Crouch 基础姿态的输入能力。
UCLASS()
class MH_RECREATE_API UMH_GA_Crouch : public UMH_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UMH_GA_Crouch();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	/** 高度（高/矮）与厚度（薄/厚）组合成四种攀爬蒙太奇。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> LowThinWallMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> LowThickWallMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HighThinWallMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HighThickWallMontage;

	/** 高度下限默认跟随角色 MaxStepHeight，超过该高度才进入攀爬检测。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MinimumObstacleHeight = 45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaximumObstacleHeight = 160.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true"))
	bool bUseCharacterMaxStepHeightAsMinimum = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float HighWallHeightThreshold = 95.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ThinWallDepthThreshold = 25.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float ForwardProbeDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float MaximumObstacleDepth = 220.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Detection", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> ClimbTraceChannel = ECC_Visibility;

	/** 三个 Notify State 使用的目标名：接近墙体、翻越顶部、墙顶前方一个身位。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Motion Warping", meta = (AllowPrivateAccess = "true"))
	FName ApproachWarpTargetName = TEXT("Climb_Approach");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Motion Warping", meta = (AllowPrivateAccess = "true"))
	FName MantleWarpTargetName = TEXT("Climb_Mantle");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Motion Warping", meta = (AllowPrivateAccess = "true"))
	FName LandingWarpTargetName = TEXT("Climb_Landing");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Motion Warping", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float WarpTargetClearance = 3.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Climb|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDrawClimbDebug = false;

	bool TryStartClimb();
	bool DetectClimbSurface(FClimbDetectionResult& OutResult);
	UAnimMontage* GetMontageForVariant(EMH_ClimbVariant Variant) const;
	EMH_ClimbVariant ClassifyClimbSurface(const FClimbDetectionResult& Detection) const;
	void ConfigureWarpTargets(const FClimbDetectionResult& Detection);
	void ClearWarpTargets();

	UFUNCTION()
	void OnClimbMontageCompleted();
	UFUNCTION()
	void OnClimbMontageCancelled();
	void RestoreMovementMode();

	TWeakObjectPtr<UMotionWarpingComponent> ActiveMotionWarpingComponent;
	TEnumAsByte<EMovementMode> PreviousMovementMode = MOVE_Walking;
	uint8 PreviousCustomMovementMode = 0;
	bool bMovementModeChanged = false;
	
};
