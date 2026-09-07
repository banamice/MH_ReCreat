// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Player/MH_GA_UnEquipWeapon.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Component/CombatComponent/MH_PawnCombatConponent.h"
#include "GameFramework/Character.h"
#include "Weapon/MH_BaseWeapon.h"
#include "Weapon/MH_ChracterWeapon.h"

void UMH_GA_UnEquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_PlayMontageAndWait* Task =  UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,FName(),UnEquipMontage);
	Task->OnBlendOut.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCompleted.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnCancelled.AddDynamic(this,&ThisClass::OnMontageEnd);
	Task->OnInterrupted.AddDynamic(this,&ThisClass::OnMontageEnd);
	
	UAbilityTask_WaitGameplayEvent* EventTask =  UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EventTag);
	EventTask->EventReceived.AddDynamic(this,&ThisClass::OnEventReceived);
	Task->ReadyForActivation();
	EventTask->ReadyForActivation();
}

void UMH_GA_UnEquipWeapon::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
}
