// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent/MH_PlayerCombatComponent.h"

#include "Weapon/MH_BaseWeapon.h"
#include "Weapon/MH_ChracterWeapon.h"


UMH_PlayerCombatComponent::UMH_PlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

AMH_ChracterWeapon* UMH_PlayerCombatComponent::GetCarryingCharacterWeaponByTag(const FGameplayTag& Tag) const
{
	 return  Cast<AMH_ChracterWeapon>(Super::GetCarryingWeaponByTag(Tag));
}

void UMH_PlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UMH_PlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

