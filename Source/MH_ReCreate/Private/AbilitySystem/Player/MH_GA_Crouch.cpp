// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Player/MH_GA_Crouch.h"

#include "BPFuncLib/MH_BluePrintFuncLib.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UMH_GA_Crouch::UMH_GA_Crouch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bRetriggerInstancedAbility = false;
}

void UMH_GA_Crouch::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AMH_BasePlayerCharacter* Character = GetPlayerCharacter();
	if (Character && Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	if (!Character)
	{
		UE_LOG(LogMH, Warning, TEXT("%s: Crouch ability has no player character"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 同一个输入在滑坡或悬崖边缘时优先执行前跳，普通地面仍然执行下蹲切换。
	if (Character->TryPerformContextualJump())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 蹲姿下第一次按键只解除蹲姿，不能在同一次输入中跳跃。
	if (Character->GetBaseGaitType() == FGaitType::Crouch || Character->IsCrouched())
	{
		if (!Character->ResetMovementStateForJump())
		{
			UE_LOG(LogMH, Warning, TEXT("%s: Failed to leave crouch before contextual jump evaluation"), *Character->GetName());
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const FGaitType TargetGait = Character->GetBaseGaitType() == FGaitType::Crouch
		? FGaitType::Walk
		: FGaitType::Crouch;
	const bool bShouldCrouch = TargetGait == FGaitType::Crouch;

	if (bShouldCrouch)
	{
		if (!Character->CanCrouch())
		{
			UE_LOG(LogMH, Warning, TEXT("%s: Crouch request rejected (CanEverCrouch=%s, IsCrouched=%s)"),
				*Character->GetName(),
				Character->GetCharacterMovement() && Character->GetCharacterMovement()->CanEverCrouch() ? TEXT("true") : TEXT("false"),
				Character->IsCrouched() ? TEXT("true") : TEXT("false"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		Character->Crouch();
	}
	else
	{
		Character->UnCrouch();
	}

	// Apply the gait while preserving the current normal or run move state.
	if (!Character->SetBaseGaitType(TargetGait))
	{
		UE_LOG(LogMH, Warning, TEXT("%s: Failed to apply gait %d while toggling crouch"),
			*Character->GetName(), static_cast<uint8>(TargetGait));
		if (bShouldCrouch)
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
