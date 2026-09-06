// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_BaseWeapon.generated.h"

class UBoxComponent;

UCLASS()
class MH_RECREATE_API AMH_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	AMH_BaseWeapon();

protected:
	//一些武器是会有动画的需要使用骨骼网格体
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
public:
	UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	
};
