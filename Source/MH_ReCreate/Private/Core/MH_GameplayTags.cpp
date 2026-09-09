// Fill out your copyright notice in the Description page of Project Settings.


#include "MH_ReCreate/Public/Core/MH_GameplayTags.h"


namespace MH_GameplayTags
{
	//InputTags
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,"InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,"InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipWeapon,"InputTag.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnEquipWeapon,"InputTag.UnEquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim,"InputTag.Aim");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_GapllingHook,"InputTag.GapllingHook");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Run,"InputTag.Run");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch,"InputTag.Crouch");
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_Y,"InputTag.Combo.Y");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_B,"InputTag.Combo.B");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_YB,"InputTag.Combo.YB");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_LT,"InputTag.Combo.LT");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_RT,"InputTag.Combo.RT");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_LTY,"InputTag.Combo.LTY");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Combo_LTB,"InputTag.Combo.LTB");
	
	
	//AbilityTag
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_LongSword,"Player.Ability.Equip.LongSword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_UnEquip_LongSword,"Player.Ability.UnEquip.LongSword");
	
	
	
	//InputTags
	UE_DEFINE_GAMEPLAY_TAG(Weapon_LongSword,"WeaponTag.LongSword");
	//EventTags
	UE_DEFINE_GAMEPLAY_TAG(Eevent_Equip_LongSword,"EventTag.Equip.LoongSword");
	UE_DEFINE_GAMEPLAY_TAG(Eevent_UnEquip_LongSword,"EventTag.UnEquip.LoongSword");
}
