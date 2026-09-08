// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BaseAnimInstance.h"
#include "MH_LocomotionParameters.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/MH_PlayerAnimInterface.h"
#include "MH_CharacterAnimInstance.generated.h"

class AMH_BaseCharacter;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_CharacterAnimInstance : public UMH_BaseAnimInstance,public IMH_PlayerAnimInterface
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	// 动画接口实现
	virtual void SetGaitType(const FGaitType InStateType) override;
	virtual void SetMoveState(const EMoveState InMoveState) override;

#pragma region Debug
public:
	// 是否显示运动调试信息、方向箭头和预测位置。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Debug", meta = (AllowPrivateAccess = "true"))
	bool bIsDebug = false;
	void Debug();
#pragma endregion

#pragma region DataForLcocomotion
public:
	// 当前动画实例所属的角色。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMH_BaseCharacter> MH_Character;
	// 当前角色使用的移动组件，提供速度、加速度和移动状态数据。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> MH_MovementComponent;

	// 上一帧的步态，用于检测步态是否发生变化。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Gait", meta = (AllowPrivateAccess = "true"))
	FGaitType LastGaitType = FGaitType::Walk;
	// 当前步态，例如 Walk 或 Crouch。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Gait", meta = (AllowPrivateAccess = "true"))
	FGaitType GaitType = FGaitType::Walk;
	// 当前移动状态，例如普通移动或奔跑。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Gait", meta = (AllowPrivateAccess = "true"))
	EMoveState MoveState = EMoveState::None;
	// 当前帧是否检测到步态相较上一帧发生变化。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Gait", meta = (AllowPrivateAccess = "true"))
	bool bGaitChanged;

	// 平面移动速度，已去除 Z 轴分量；根运动抑制期间会被置零。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector VelocityXY;
	// 角色移动组件提供的完整世界空间速度，包含 Z 轴分量。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector VelocityXYZ;
	// 当前帧的平面加速度，已去除 Z 轴分量，使用世界空间。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector Acceleration2D;
	// 上一帧保存的平面加速度，用于判断加速度方向是否改变。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	FVector LastAcceleration2D;
	// 当前是否存在有效的平面加速度输入。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;
	// 当前帧加速度方向是否与上一帧处于不同的大方向。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	bool bAccelerationChange = false;
	// 加速度方向与角色前方向之间的夹角，单位为度，范围为 0 到 180。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	float AccelerationDirectionAngle = 0.0f;
	// 根据角色朝向分类后的加速度方向：前、后、左、右或 None。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	EMovementDirection AccelerationDirection = EMovementDirection::None;
	// 加速度方向与当前速度方向之间的夹角，单位为度。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	float AccelerationVelocityAngle = 0.0f;
	// 根据加速度相对速度方向分类后的方向：前、后、左、右或 None。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	EMovementDirection AccelerationVelocityDirection = EMovementDirection::None;
	// 加速度方向发生变化后，记录需要切换到的目标方向。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Speed", meta = (AllowPrivateAccess = "true"))
	EMovementDirection ChangeDirectionTo = EMovementDirection::None;
	// 角色前方向的二维单位向量，世界空间。
	FVector CharacterForward2D = FVector::ForwardVector;
	// 角色右方向的二维单位向量，世界空间。
	FVector CharacterRight2D = FVector::RightVector;

	// 当前是否有动画根运动正在驱动角色移动。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|RootMotion", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayingRootMotion = false;
	// 是否暂时屏蔽移动速度对移动状态机的驱动，避免根运动速度误触发 Cycle 或 Stop。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|RootMotion", meta = (AllowPrivateAccess = "true"))
	bool bIsLocomotionVelocitySuppressed = false;

	// 当前帧根据平面速度计算出的位移增量，单位为厘米。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|DistanceMatch", meta = (AllowPrivateAccess = "true"))
	float DeltaDistance = 0.0f;
	// 根据当前速度和制动参数预测的停止位移，世界空间二维向量。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|DistanceMatch", meta = (AllowPrivateAccess = "true"))
	FVector PredictStopDistance ;
	// 根据当前加速度和速度预测的转向位移，世界空间二维向量。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|DistanceMatch", meta = (AllowPrivateAccess = "true"))
	FVector PredictPivotDistance ;

	// 角色当前是否处于跳跃阶段。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Jump", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping = false ;
	// 角色当前是否正在下落。
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Jump", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false ;
	UPROPERTY(BlueprintReadOnly, Category = "MH|Lcocomotion|Jump", meta = (AllowPrivateAccess = "true"))
	bool bIsOnAir = false ;
#pragma endregion
};
