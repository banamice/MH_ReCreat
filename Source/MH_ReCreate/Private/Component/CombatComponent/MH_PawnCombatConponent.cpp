// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent/MH_PawnCombatConponent.h"

#include "BPFuncLib/MH_BluePrintFuncLib.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/MH_BaseWeapon.h"

void UMH_PawnCombatConponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMH_PawnCombatConponent, HoldWeaponTag);
	DOREPLIFETIME(UMH_PawnCombatConponent, bWeaponEquipMontageFinished);
	DOREPLIFETIME(UMH_PawnCombatConponent, CarryingWeapons);
}

void UMH_PawnCombatConponent::RegisterCarryWeapon(const FGameplayTag& WeaponTag, AMH_BaseWeapon* CarryWeapon,
	bool bRegisterHold)
{
	if (!WeaponTag.IsValid() || !IsValid(CarryWeapon))
	{
		UE_LOG(LogMH, Warning, TEXT("RegisterCarryWeapon ignored: invalid tag (%s) or weapon (%s)"),
			*WeaponTag.ToString(), IsValid(CarryWeapon) ? *CarryWeapon->GetName() : TEXT("None"));
		return;
	}

	FMHCarriedWeaponEntry* ExistingEntry = CarryingWeapons.FindByPredicate(
		[&WeaponTag](const FMHCarriedWeaponEntry& Entry)
		{
			return Entry.WeaponTag == WeaponTag;
		});

	if (ExistingEntry)
	{
		ExistingEntry->CarryWeapon = CarryWeapon;
	}
	else
	{
		FMHCarriedWeaponEntry& NewEntry = CarryingWeapons.AddDefaulted_GetRef();
		NewEntry.WeaponTag = WeaponTag;
		NewEntry.CarryWeapon = CarryWeapon;
	}

	if (bRegisterHold)
	{
		HoldWeaponTag = WeaponTag;
	}

	UE_LOG(LogMH, Verbose, TEXT("Registered carry weapon %s with tag %s on %s%s"),
		*CarryWeapon->GetName(), *WeaponTag.ToString(), *GetName(), bRegisterHold ? TEXT(" (hold weapon)") : TEXT(""));
}

AMH_BaseWeapon* UMH_PawnCombatConponent::GetCarryingWeaponByTag(const FGameplayTag& WeaponTag) const
{
	if (!WeaponTag.IsValid())
	{
		// 空标签表示当前没有查询目标（例如角色尚未装备武器），这是正常状态。
		return nullptr;
	}

	if (const FMHCarriedWeaponEntry* Entry = CarryingWeapons.FindByPredicate(
		[&WeaponTag](const FMHCarriedWeaponEntry& Candidate)
		{
			return Candidate.WeaponTag == WeaponTag;
		}))
	{
		if (IsValid(Entry->CarryWeapon.Get()))
		{
			return Entry->CarryWeapon.Get();
		}
	}

	UE_LOG(LogMH, Verbose, TEXT("No carrying weapon with tag %s on %s (registered: %d)"),
		*WeaponTag.ToString(), *GetName(), CarryingWeapons.Num());
	return nullptr;
}

AMH_BaseWeapon* UMH_PawnCombatConponent::GetHoldWeapon() const
{
	if (!HoldWeaponTag.IsValid())
	{
		// 未持有武器时不进入通用查找，避免动画实例每帧产生无意义日志。
		return nullptr;
	}
	return GetCarryingWeaponByTag(HoldWeaponTag);
}

void UMH_PawnCombatConponent::SetWeaponEquipMontageFinished(const bool bFinished)
{
	// 装备能力在拥有端和服务器端都会收到蒙太奇回调；两端都更新本地状态，
	// 服务器值通过属性复制同步给其它客户端，避免仅依赖本地动画时序。
	bWeaponEquipMontageFinished = bFinished;
}

bool UMH_PawnCombatConponent::IsWeaponEquipMontageFinished() const
{
	return bWeaponEquipMontageFinished;
}
