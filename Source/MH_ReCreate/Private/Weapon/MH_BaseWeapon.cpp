// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/MH_BaseWeapon.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AMH_BaseWeapon::AMH_BaseWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(SkeletalMeshComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FTransform AMH_BaseWeapon::GetLeftHandIKTransform() const
{
	return SkeletalMeshComponent && SkeletalMeshComponent->DoesSocketExist(TEXT("LeftHandIK"))
		? SkeletalMeshComponent->GetSocketTransform(TEXT("LeftHandIK"), RTS_World)
		: FTransform::Identity;
}

FTransform AMH_BaseWeapon::GetLeftHandIKComponentTransform() const
{
	return SkeletalMeshComponent && SkeletalMeshComponent->DoesSocketExist(TEXT("LeftHandIK"))
		? SkeletalMeshComponent->GetSocketTransform(TEXT("LeftHandIK"), RTS_Component)
		: FTransform::Identity;
}

bool AMH_BaseWeapon::HasLeftHandIKSocket() const
{
	return SkeletalMeshComponent && SkeletalMeshComponent->DoesSocketExist(TEXT("LeftHandIK"));
}



