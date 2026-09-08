// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locomotion/MH_BaseAnimInstance.h"
#include "MH_LinkAnimLayer.generated.h"

class UMH_PlayerAnimInstance;
/**
 * 
 */
UCLASS()
class MH_RECREATE_API UMH_LinkAnimLayer : public UMH_BaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "MH|Animation", meta = (BlueprintThreadSafe))
	UMH_PlayerAnimInstance* GetPlayerAnimInstance() const;
	
	
	
#pragma region LocomitionAmin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Idle")
	TObjectPtr<UAnimSequenceBase> IdleWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Idle")
	TObjectPtr<UAnimSequenceBase> IdleCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Idle")
	TObjectPtr<UAnimSequenceBase> IdleCrouchStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Idle")
	TObjectPtr<UAnimSequenceBase> IdleCrouchEnd;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	float WalkSlowThreadHold = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	TObjectPtr<UAnimSequenceBase> CycleWalkSlow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	TObjectPtr<UAnimSequenceBase> CycleWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	TObjectPtr<UAnimSequenceBase> CycleWalkRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	TObjectPtr<UAnimSequenceBase> CycleCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Cycle")
	TObjectPtr<UAnimSequenceBase> CycleCrouchRun;
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Stop")
	TObjectPtr<UAnimSequenceBase> StopWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Stop")
	TObjectPtr<UAnimSequenceBase> StopWalkRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Stop")
	TObjectPtr<UAnimSequenceBase> StopCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Stop")
	TObjectPtr<UAnimSequenceBase> StopCrouchRun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartWalkForWard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartWalkLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartWalkRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartWalkBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartCrouchForWard;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchBack;
#pragma endregion

};
