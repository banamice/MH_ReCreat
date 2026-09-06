// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/MH_BaseActorComponent.h"
#include "MH_PawnCombatConponent.generated.h"


class AMH_BaseWeapon;

USTRUCT()
struct FMHCarriedWeaponEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag WeaponTag;

	UPROPERTY()
	TObjectPtr<AMH_BaseWeapon> CarryWeapon = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_PawnCombatConponent : public UMH_BaseActorComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable,Category="MH|Combat")
	void RegisterCarryWeapon(const FGameplayTag& WeaponTag, AMH_BaseWeapon* CarryWeapon, bool bRegisterHold = false);
	UFUNCTION(BlueprintCallable,Category="MH|Combat")
	AMH_BaseWeapon* GetCarryingWeaponByTag(const FGameplayTag& WeaponTag) const;
	UFUNCTION(BlueprintCallable,Category="MH|Combat")
	AMH_BaseWeapon* GetHoldWeapon()const;

	UPROPERTY(Replicated, BlueprintReadWrite,Category="MH|Combat")
	FGameplayTag HoldWeaponTag;
private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "MH|Combat")
	TArray<FMHCarriedWeaponEntry> CarryingWeapons;
};
