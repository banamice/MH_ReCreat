// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_EquipWeapon.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/MH_BaseWeapon.h"
#include "Weapon/MH_ChracterWeapon.h"

void UMH_GA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		// 武器可能在蒙太奇中段就通过动画通知挂到右手，此时仍禁止左手 IK。
		CombatComponent->SetWeaponEquipMontageFinished(false);
	}

	if (AMH_BasePlayerCharacter* Character = GetPlayerCharacter())
	{
		const bool bIsCrouched = Character->GetBaseGaitType() == FGaitType::Crouch
			|| (Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsCrouching());
		if (bIsCrouched)
		{
			Character->UnCrouch();
			if (!Character->SetBaseGaitType(FGaitType::Walk))
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
		}
	}

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName(), EquipMontage, 1.f, NAME_None, false);
	Task->OnCompleted.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCancelled.AddDynamic(this,&ThisClass::OnMontageCancelled);
	Task->OnInterrupted.AddDynamic(this,&ThisClass::OnMontageCancelled);
	
	UAbilityTask_WaitGameplayEvent* EventTask =  UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EventTag);
	EventTask->EventReceived.AddDynamic(this,&ThisClass::OnEventReceived);
	Task->ReadyForActivation();
	EventTask->ReadyForActivation();
}

void UMH_GA_EquipWeapon::OnMontageEnd()
{
	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		// 只有完整播放到结尾才允许动画实例在下一帧启用左手 IK。
		CombatComponent->SetWeaponEquipMontageFinished(true);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMH_GA_EquipWeapon::OnMontageCancelled()
{
	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		CombatComponent->SetWeaponEquipMontageFinished(false);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMH_GA_EquipWeapon::OnEventReceived(FGameplayEventData Payload)
{
	check(WeaponTag.IsValid())
	AMH_ChracterWeapon* Weapon = Cast<AMH_ChracterWeapon>(GetCombatComponent()->GetCarryingWeaponByTag(WeaponTag)) ;
	if (!Weapon ) return;
	
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget,false);
	Rules.ScaleRule=EAttachmentRule::KeepWorld;
	Weapon->AttachToComponent(CurrentActorInfo->SkeletalMeshComponent.Get(),Rules,HoldSocketName);
	GetCombatComponent()->HoldWeaponTag = WeaponTag;
	
	//调用武器的的初始化函数,为玩家赋予武器相关能力，动画层，IMC
	Weapon->SetupWeaponSetFromDA(CurrentActorInfo->AbilitySystemComponent.Get(),CurrentActorInfo->PlayerController.Get(),
		Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()),1);
}
