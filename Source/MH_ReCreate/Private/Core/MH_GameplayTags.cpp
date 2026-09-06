// Fill out your copyright notice in the Description page of Project Settings.


#include "MH_ReCreate/Public/Core/MH_GameplayTags.h"


namespace MH_GameplayTags
{
	//InputTags
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,"InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,"InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipWeapon,"InputTag.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnEquipWeapon,"InputTag.UnEquipWeapon");
	
	//InputTags
	UE_DEFINE_GAMEPLAY_TAG(Weapon_LongSword,"WeaponTag.LongSword");
	//EventTags
	UE_DEFINE_GAMEPLAY_TAG(Eevent_Equip_LongSword,"EventTag.Equip.LoongSword");
	UE_DEFINE_GAMEPLAY_TAG(Eevent_UnEquip_LongSword,"EventTag.UnEquip.LoongSword");
}