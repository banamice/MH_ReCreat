// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_PawnCombatConponent.h"
#include "MH_PlayerCombatComponent.generated.h"


class AMH_ChracterWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_PlayerCombatComponent : public UMH_PawnCombatConponent
{
	GENERATED_BODY()

public:
	UMH_PlayerCombatComponent();
	AMH_ChracterWeapon* GetCarryingCharacterWeaponByTag(const FGameplayTag& Tag) const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
