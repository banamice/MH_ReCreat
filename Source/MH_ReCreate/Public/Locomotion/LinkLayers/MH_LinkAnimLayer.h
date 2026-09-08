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
	TObjectPtr<UAnimSequenceBase> StartWalkBackL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartWalkBackR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Start")
	TObjectPtr<UAnimSequenceBase> StartCrouchForWard;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkBackL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotWalkBackR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchBackL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Pivot")
	TObjectPtr<UAnimSequenceBase> PivotCrouchBackR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpStartRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpStartWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpStartLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpApex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpFallLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandNone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandFwd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandBwdL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MH|Jump")
	TObjectPtr<UAnimSequenceBase> JumpLandBwdR;
#pragma endregion

};
