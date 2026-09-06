// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BaseWeapon.h"
#include "MH_WeaponDataStruct.h"
#include "MH_ChracterWeapon.generated.h"

UCLASS()
class MH_RECREATE_API AMH_ChracterWeapon : public AMH_BaseWeapon
{
	GENERATED_BODY()

public:
	AMH_ChracterWeapon();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="MH|WeaponSet")
	FWeaponData WeaponData;
};
