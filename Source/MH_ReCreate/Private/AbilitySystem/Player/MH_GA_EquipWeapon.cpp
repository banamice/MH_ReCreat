// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_EquipWeapon.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"
#include "GameFramework/Character.h"
#include "Weapon/MH_BaseWeapon.h"
#include "Weapon/MH_ChracterWeapon.h"

void UMH_GA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_PlayMontageAndWait* Task =  UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,FName(),EquipMontage);
	Task->OnBlendOut.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCompleted.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCancelled.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnInterrupted.AddDynamic(this,&ThisClass::OnMontageEnd);
	
	UAbilityTask_WaitGameplayEvent* EventTask =  UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EventTag);
	EventTask->EventReceived.AddDynamic(this,&ThisClass::OnEventReceived);
	Task->ReadyForActivation();
	EventTask->ReadyForActivation();
}

void UMH_GA_EquipWeapon::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
