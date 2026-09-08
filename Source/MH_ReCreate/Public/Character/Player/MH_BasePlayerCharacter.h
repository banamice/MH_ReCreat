// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Locomotion/MH_LocomotionParameters.h"
#include "MH_ReCreate/Public/Character/MH_BaseCharacter.h"
#include "MH_BasePlayerCharacter.generated.h"

class UMH_DA_GaitLocomotionParams;
class UMH_PlayerCombatComponent;
struct FInputActionValue;
class UMH_DA_Input;
class UCameraComponent;
class USpringArmComponent;
class UMH_CharacterMovementComponent;

/** 最近一次蹲下输入最终执行的动作。 */
UENUM(BlueprintType)
enum class ECrouchInputAction : uint8
{
	None,
	Crouch,
	Jump,
};

/** 最近一次蹲下输入被改为跳跃的原因。 */
UENUM(BlueprintType)
enum class ECrouchInputReason : uint8
{
	None,
	NormalGround,
	Sliding,
	Ledge,
};

UCLASS()
class MH_RECREATE_API AMH_BasePlayerCharacter : public AMH_BaseCharacter
{
	GENERATED_BODY()

public:
	AMH_BasePlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

protected:
	
	
private:
#pragma  region Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_PlayerCombatComponent> CombatComponent;
#pragma endregion
	
#pragma region Locomotion
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Lcocomotion",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_DA_GaitLocomotionParams> LocomotionParams;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	FGaitType GaitType = FGaitType::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	FGaitType BaseGaitType = FGaitType::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsRunning = false;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	EMoveState MoveState = EMoveState::None;

	void OnGaitTypeChange(const FGaitType InGaitType);
	bool SetBaseGaitType(const FGaitType InBaseGaitType);
	bool SetRunState(bool bInRunning);
	/** Jumping always uses the normal standing locomotion configuration. */
	UFUNCTION(BlueprintCallable, Category = "MH|Movement")
	bool ResetMovementStateForJump();
	FGaitType GetBaseGaitType() const { return BaseGaitType; }
	bool IsRunning() const { return bIsRunning; }

	/** 返回玩家使用的自定义移动组件，供 C++ 和蓝图访问滑行数据。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement")
	UMH_CharacterMovementComponent* GetMHCharacterMovementComponent() const;

	/** 返回当前是否正在沿陡坡向下滑行。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	bool IsSliding() const;

	/** 返回当前地面是否满足滑行坡度条件。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	bool CanSlide() const;

	/** 返回当前接触地面的坡度角，单位为度。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	float GetGroundSlopeAngle() const;
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Context")
	ECrouchInputAction GetPredictedCrouchInputAction() const;
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Context")
	ECrouchInputReason GetPredictedCrouchInputReason() const;

	/** 返回当前沿下坡方向的速度，单位为厘米/秒。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	float GetSlideSpeed() const;

	/** 返回当前地面的下坡方向，世界空间单位向量。 */
	UFUNCTION(BlueprintPure, Category = "MH|Movement|Slide")
	FVector GetSlideDirection() const;

	/**
	 * 查询前方是否存在满足条件的悬崖边缘，并返回前跳速度。
	 * 边缘落差必须达到角色胶囊体完整高度，不设置可配置的最大落差。
	 */
	UFUNCTION(BlueprintCallable, Category = "MH|Movement|Ledge Jump")
	bool TryGetForwardLedgeJumpVelocity(FVector& OutJumpVelocity);

	/** 根据当前环境执行滑坡跳跃或悬崖边缘前跳。 */
	UFUNCTION(BlueprintCallable, Category = "MH|Movement|Ledge Jump")
	bool TryPerformContextualJump();

	/** 最近一次边缘检测结果，供动画蓝图或调试蓝图读取。 */
	UPROPERTY(BlueprintReadOnly, Category = "MH|Movement|Ledge Jump")
	bool bLedgeJumpAvailable = false;

	/** 最近一次边缘检测得到的落差，单位为厘米。 */
	UPROPERTY(BlueprintReadOnly, Category = "MH|Movement|Ledge Jump")
	float LedgeDropHeight = 0.0f;

	/** 最近一次边缘检测计算出的跳跃速度。 */
	UPROPERTY(BlueprintReadOnly, Category = "MH|Movement|Ledge Jump")
	FVector LedgeJumpVelocity = FVector::ZeroVector;

private:
	bool ApplyLocomotionState(const FGaitType InBaseGaitType, const EMoveState InMoveState);
	bool FindForwardLedgeDrop(FHitResult& OutHitResult, float& OutDropHeight) const;
	void ResetLedgeJumpState();

	/** 前方采样距离。该值不是悬崖最大落差。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float LedgeProbeDistance = 75.0f;

	/** 前跳的水平速度，单位为厘米/秒。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float LedgeJumpForwardSpeed = 600.0f;

	/** 前跳的垂直速度，单位为厘米/秒。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float LedgeJumpUpSpeed = 420.0f;

	/** 边缘和落点检测使用的碰撞通道。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MH|Movement|Ledge Jump", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> LedgeTraceChannel = ECC_Visibility;
#pragma endregion
	
#pragma  region Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_DA_Input> InputDA;
	
	void BindInput(UInputComponent* PlayerInputComponent);
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void AbilityPressed(FGameplayTag AbilityTag);
	void AbilityReleased(FGameplayTag AbilityTag);
#pragma endregion
#pragma region Getter Setter
public:
	UFUNCTION(BlueprintPure, Category = "MH|Combat")
	UMH_PlayerCombatComponent* GetMHCombatComponent() const;
#pragma endregion
};
