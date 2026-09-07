// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "MH_BaseWeapon.h"
#include "MH_WeaponDataAsset.h"
#include "MH_WeaponDataStruct.h"
#include "MH_ChracterWeapon.generated.h"

UCLASS()
class MH_RECREATE_API AMH_ChracterWeapon : public AMH_BaseWeapon
{
	GENERATED_BODY()

public:
	AMH_ChracterWeapon(); 

	//为ASC添加能力，添加IMC，链接动画层
	UFUNCTION(BlueprintCallable)
	void SetupWeaponSetFromDA(UAbilitySystemComponent* InASC,
		APlayerController* InPlayerController,
		ACharacter* InPlayerCharacter,
		int32 AbilityLevel);
	
	UFUNCTION(BlueprintCallable)
	void RemoveWeaponSetFromDA(UAbilitySystemComponent* InASC,
		APlayerController* InPlayerController,
		ACharacter* InPlayerCharacter);
	
	
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="MH|WeaponSet",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMH_WeaponDataAsset> WeaponDataAsset ; 
	
	//装备武器时授予给ASC的能力句柄。需要在取消装备时移除
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="MH|Ability",meta=(AllowPrivateAccess="true"))
	TArray<FGameplayAbilitySpecHandle>  GrantedAbilities;
};
