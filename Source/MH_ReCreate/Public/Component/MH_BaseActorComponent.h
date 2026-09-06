// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MH_BaseActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MH_RECREATE_API UMH_BaseActorComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	template<class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,"T must be a subclass of APawn");
		return Cast<T>(GetOwner());
	}
	
	APawn* GetOwningPawn() const
	{
		return Cast<APawn>(GetOwner());
	}
	
	template<class T>
	T* GetOwnController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value,"T must be a subclass of AController");
		return Cast<T>(GetOwningPawn()->GetController());
	}

};
