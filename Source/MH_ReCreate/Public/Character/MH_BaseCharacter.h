// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Locomotion/LinkLayers/MH_LinkAnimLayer.h"
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
	AMH_BaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ Begin IAbilitySystemInterface Interface.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; ;
	//~ End IAbilitySystemInterface Interface
	
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
	virtual void OnRep_Controller() override;
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_BaseAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMH_BaseAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MH|Setup",meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMH_BaseSetupDataAsset> SetupDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MH|Setup",meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMH_LinkAnimLayer> DefaultLinkAnimLayer;
	
#pragma region  Getter Setter
public:
	FORCEINLINE UMH_BaseAbilitySystemComponent* GetMHAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE UMH_BaseAttributeSet* GetAttributeSet() const { return AttributeSet; }
	FORCEINLINE TSubclassOf<UMH_LinkAnimLayer> GetDefaultLinkAnimLayer() const { return DefaultLinkAnimLayer; }
#pragma endregion
};

