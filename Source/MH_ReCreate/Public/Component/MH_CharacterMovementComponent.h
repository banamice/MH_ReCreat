// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MH_CharacterMovementComponent.generated.h"

/**
 * 玩家移动组件的地面状态扩展。
 *
 * 组件不改变 CharacterMovementComponent 的基础移动规则，只维护滑行判定所需的
 * 地面法线、坡度角和沿坡速度，供角色、能力和动画蓝图复用。
 */
UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UMH_CharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 最小滑行坡度。坡度角达到该值后，才具备滑行条件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Movement|Slide", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float SlideStartAngle = 35.0f;

	/** 判定为正在滑行所需的最小沿坡速度。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Movement|Slide", meta = (ClampMin = "0.0"))
	float SlideStartSpeed = 25.0f;

	/** 当前地面是否满足滑行的坡度条件。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Slide")
	bool bCanSlide = false;

	/** 当前是否正在沿坡向下移动。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Slide")
	bool bIsSliding = false;

	/** 当前接触地面的坡度角，单位为度。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Slide")
	float GroundSlopeAngle = 0.0f;

	/** 当前速度在下坡方向上的分量，单位为厘米/秒。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Slide")
	float SlideSpeed = 0.0f;

	/** 当前地面对应的下坡方向，世界空间单位向量。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Slide")
	FVector SlideDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump")
	bool bCanPerformLedgeJump = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump")
	float LedgeDropHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Movement|Ledge Jump", meta = (ClampMin = "1.0"))
	float LedgeProbeDistance = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Movement|Ledge Jump")
	TEnumAsByte<ECollisionChannel> LedgeTraceChannel = ECC_Visibility;

	/** 返回当前是否满足滑行坡度条件。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	bool CanSlide() const { return bCanSlide; }

	/** 返回当前是否正在滑行。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	bool IsSliding() const { return bIsSliding; }

	UFUNCTION(BlueprintPure, Category = "MH|Movement|Ledge Jump")
	bool CanPerformLedgeJump() const { return bCanPerformLedgeJump; }

	/** 手动刷新一次地面滑行状态，适合输入事件或能力激活时调用。 */
	UFUNCTION(BlueprintCallable, Category = "MH|Movement|Slide")
	void RefreshSurfaceState();

protected:
	virtual void OnMovementUpdated(
		float DeltaSeconds,
		const FVector& OldLocation,
		const FVector& OldVelocity) override;

private:
	/** 根据 CharacterMovementComponent 当前的 FloorResult 更新所有滑行数据。 */
	void UpdateSurfaceState();
	void ResetSurfaceState();
	void UpdateLedgeState();
	void ResetLedgeState();
};
