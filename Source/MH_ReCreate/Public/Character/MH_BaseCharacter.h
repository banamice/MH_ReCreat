// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MH_BaseCharacter.generated.h"

class UMH_BaseSetupDataAsset;
class UMH_BaseAbilitySystemComponent;
class UMH_BaseAttributeSet;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class MH_RECREATE_API AMH_BaseCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMH_BaseCharacter();
	
	//~ Begin IAbilitySystemInterface Interface.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; ;
	//~ End IAbilitySystemInterface Interface
	
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
	virtual void OnRep_Controller() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_BaseAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_BaseAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MH|Setup",meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMH_BaseSetupDataAsset> SetupDataAsset;
	
#pragma region  Getter Setter
public:
	FORCEINLINE UMH_BaseAbilitySystemComponent* GetMHAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE UMH_BaseAttributeSet* GetAttributeSet() const { return AttributeSet; }
#pragma endregion
};

