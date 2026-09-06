// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/MH_BaseWeapon.h"

#include "Components/BoxComponent.h"


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



