// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_BaseWeapon.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class MH_RECREATE_API AMH_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	AMH_BaseWeapon();

	/** 返回武器网格上的左手握持 Socket（LeftHandIK）的世界空间变换。 */
	UFUNCTION(BlueprintPure, Category = "MH|Weapon|IK")
	FTransform GetLeftHandIKTransform() const;

	/** 返回武器网格上的左手握持 Socket（LeftHandIK）的武器组件空间变换。 */
	FTransform GetLeftHandIKComponentTransform() const;

	/** 判断武器网格是否配置了左手握持 Socket。 */
	UFUNCTION(BlueprintPure, Category = "MH|Weapon|IK")
	bool HasLeftHandIKSocket() const;

protected:
	//一些武器是会有动画的需要使用骨骼网格体
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
public:
	UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	
};
