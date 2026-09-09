// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_UnEquipWeapon.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"
#include "GameFramework/Character.h"
#include "Weapon/MH_BaseWeapon.h"
#include "Weapon/MH_ChracterWeapon.h"

void UMH_GA_UnEquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		// 卸除流程开始后立即关闭左手 IK，避免继续吸附到即将收回的武器。
		CombatComponent->SetWeaponEquipMontageFinished(false);
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName(), UnEquipMontage, 1.f, NAME_None, false);
	Task->OnCompleted.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCancelled.AddDynamic(this,&ThisClass::OnMontageCancelled);
	Task->OnInterrupted.AddDynamic(this,&ThisClass::OnMontageCancelled);
	
	UAbilityTask_WaitGameplayEvent* EventTask =  UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EventTag);
	EventTask->EventReceived.AddDynamic(this,&ThisClass::OnEventReceived);
	Task->ReadyForActivation();
	EventTask->ReadyForActivation();
}

void UMH_GA_UnEquipWeapon::OnMontageEnd()
{
	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		CombatComponent->SetWeaponEquipMontageFinished(false);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMH_GA_UnEquipWeapon::OnMontageCancelled()
{
	if (UMH_PawnCombatConponent* CombatComponent = GetCombatComponent())
	{
		CombatComponent->SetWeaponEquipMontageFinished(false);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UMH_GA_UnEquipWeapon::OnEventReceived(FGameplayEventData Payload)
{
	AMH_ChracterWeapon* Weapon = Cast<AMH_ChracterWeapon>(GetCombatComponent()->GetHoldWeapon()) ;
	if (!Weapon) return;
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget,false);
	Rules.ScaleRule=EAttachmentRule::KeepWorld;
	Weapon->AttachToComponent(CurrentActorInfo->SkeletalMeshComponent.Get(),Rules,CarrySocketName);
	GetCombatComponent()->HoldWeaponTag = FGameplayTag();
	
	//调用武器的移除函数来移除IMC,能力，动画层
	Weapon->RemoveWeaponSetFromDA(CurrentActorInfo->AbilitySystemComponent.Get(),CurrentActorInfo->PlayerController.Get(),
		Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()));
	
	//调用player的设置运动参数函数，将运动参数重置回walk状态
	Cast<AMH_BasePlayerCharacter>(CurrentActorInfo->AvatarActor.Get())->OnGaitTypeChange(FGaitType::Walk);
}
