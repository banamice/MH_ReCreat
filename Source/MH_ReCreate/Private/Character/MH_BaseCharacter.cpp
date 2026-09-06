// Fill out your copyright notice in the Description page of Project Settings.


#include "MH_ReCreate/Public/Character/MH_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "AbilitySystem/MH_BaseAttributeSet.h"


// Sets default values
AMH_BaseCharacter::AMH_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	GetMesh()->bReceivesDecals = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UMH_BaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UMH_BaseAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AMH_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AMH_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!HasAuthority()) return;
	check(AbilitySystemComponent)
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void AMH_BaseCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

