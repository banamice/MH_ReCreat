// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace MH_GameplayTags
{
	//InputTags
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_EquipWeapon);
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnEquipWeapon);
	
	//InputTags
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_LongSword);
	//EventTags
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eevent_Equip_LongSword);
	MH_RECREATE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eevent_UnEquip_LongSword);
	
}