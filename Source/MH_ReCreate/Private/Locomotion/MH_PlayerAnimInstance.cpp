// Fill out your copyright notice in the Description page of Project Settings.


#include "Locomotion/MH_PlayerAnimInstance.h"

#include "Character/Player/MH_BasePlayerCharacter.h"
#include "Component/CombatComponent/MH_PlayerCombatComponent.h"

void UMH_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	NativeUpdateAnimation(0.0f);
}

void UMH_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AMH_BasePlayerCharacter* Player = Cast<AMH_BasePlayerCharacter>(TryGetPawnOwner());
	if (MH_Player != Player)
	{
		MH_Player = Player;
	}

	UMH_PlayerCombatComponent* CombatComponent =
		IsValid(MH_Player) ? MH_Player->GetMHCombatComponent() : nullptr;
	if (MH_CombatComponent != CombatComponent)
	{
		MH_CombatComponent = CombatComponent;
	}
}
