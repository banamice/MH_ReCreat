// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Share/MH_GA_SpawnWeapon.h"

#include "Component/CombatComponent/MH_PawnCombatConponent.h"
#include "Weapon/MH_ChracterWeapon.h"

UMH_GA_SpawnWeapon::UMH_GA_SpawnWeapon()
{
	// Defaults matching the intended ability class settings in the editor.
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	bServerRespectsRemoteAbilityCancellation = true;
	bRetriggerInstancedAbility = false;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UMH_GA_SpawnWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	check(WeaponClass);
	AMH_BaseWeapon* Actor =  Cast<AMH_BaseWeapon>(GetWorld()->SpawnActor(WeaponClass));
	check(Actor);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget,false);
	Rules.ScaleRule=EAttachmentRule::KeepWorld;
	Actor->AttachToComponent(ActorInfo->SkeletalMeshComponent.Get(),Rules,SpawnSocketName);
	
	GetCombatComponent()->RegisterCarryWeapon(WeaponTag,Actor,RegisterHold);
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
