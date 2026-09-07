// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/MH_ChracterWeapon.h"
#include "Weapon/MH_WeaponDataAsset.h"
#include "Weapon/MH_WeaponDataStruct.h"
#include "Locomotion/LinkLayers/MH_LinkAnimLayer.h"
#include "AbilitySystem/Player/MH_PlayerGameplayAbility.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Core/MH_BasePlayerController.h"
#include "Components/BoxComponent.h"


// Sets default values
AMH_ChracterWeapon::AMH_ChracterWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMH_ChracterWeapon::SetupWeaponSetFromDA(UAbilitySystemComponent* InASC, APlayerController* InPlayerController,
	ACharacter* InPlayerCharacter, int32 AbilityLevel)
{
	if (!IsValid(InASC) || !IsValid(InPlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetupWeaponFromDA failed: invalid ASC or player character"));
		return;
	}
	
	//切换链接动画层
	if (WeaponDataAsset->WeaponData.LinkAnimLayer)
	{
		InPlayerCharacter->GetMesh()->LinkAnimClassLayers(WeaponDataAsset->WeaponData.LinkAnimLayer);
	}
	
	//添加imc
	if (IsValid(InPlayerController) && InPlayerController->IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (IsValid(WeaponDataAsset->WeaponData.WeaponIMC.Get()))
				{
					InputLocalPlayerSubsystem->AddMappingContext(WeaponDataAsset->WeaponData.WeaponIMC, 1);
				}
			}
		}
	}

	UMH_BaseAbilitySystemComponent* InMHASC = Cast<UMH_BaseAbilitySystemComponent>(InASC);
	if (!IsValid(InMHASC) || !InMHASC->GetOwner() || !InMHASC->GetOwner()->HasAuthority())
	{
		return;
	}
	
	//授予能力
	for (const auto& InputAbility : WeaponDataAsset->WeaponData.GrantedAbilities)
	{
		if (!InputAbility.IsValid()) continue;

		
		FGameplayAbilitySpec Spec(InputAbility.Ability,AbilityLevel);
		Spec.SourceObject = InMHASC->GetAvatarActor();
		Spec.Level = AbilityLevel;
		//不一定需要添加tag、战斗状态其实不依靠inputTag激活能力。只有非战斗状态，比如说家园里的翔虫之类的没有固定状态的。
		//使用输入绑定激活
		if (InputAbility.GameplayTag.IsValid())
		{
			Spec.DynamicAbilityTags.AddTag(InputAbility.GameplayTag);
		}
		GrantedAbilities.Add(InMHASC->GiveAbility(Spec));
	}
	
	//后续还需要添加注册input回调
	
	//还需要设置运动数据
}

void AMH_ChracterWeapon::RemoveWeaponSetFromDA(UAbilitySystemComponent* InASC, APlayerController* InPlayerController,
	ACharacter* InPlayerCharacter)
{
	if (!IsValid(InASC) || !IsValid(InPlayerCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveWeaponSetFromDA failed: invalid ASC or player character"));
		return;
	}
	
	//取消链接动画层
	if (WeaponDataAsset->WeaponData.LinkAnimLayer)
	{
		InPlayerCharacter->GetMesh()->UnlinkAnimClassLayers(WeaponDataAsset->WeaponData.LinkAnimLayer);
	}
	
	//取消imc
	if (IsValid(InPlayerController) && InPlayerController->IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (IsValid(WeaponDataAsset->WeaponData.WeaponIMC.Get()))
				{
					InputLocalPlayerSubsystem->RemoveMappingContext(WeaponDataAsset->WeaponData.WeaponIMC);
				}
			}
		}
	}

	UMH_BaseAbilitySystemComponent* InMHASC = Cast<UMH_BaseAbilitySystemComponent>(InASC);
	if (!IsValid(InMHASC) || !InMHASC->GetOwner() || !InMHASC->GetOwner()->HasAuthority())
	{
		return;
	}
	
	//clear能力
	for (const auto& InputAbility : GrantedAbilities)
	{
		if (!InputAbility.IsValid()) continue;

		InMHASC->ClearAbility(InputAbility);
	}
	GrantedAbilities.Empty();
	
	//后续还需要删除input回调
	
	//还需要更新运动数据
}


