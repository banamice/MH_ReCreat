// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Player/MH_PlayerSetupDataAsset.h"
#include "Locomotion/MH_LocomotionParameters.h"
#include "UObject/Object.h"
#include "MH_WeaponDataStruct.generated.h"

class UInputMappingContext;
class UMH_LinkAnimLayer;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLocomotionParameters LocomotionParameters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMH_LinkAnimLayer> LinkAnimLayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> WeaponIMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputAbility> GrantedAbilities;
};
