// Fill out your copyright notice in the Description page of Project Settings.


#include "MH_ReCreate/Public/Character/Player/MH_BasePlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/MH_BaseAbilitySystemComponent.h"
#include "AbilitySystem/MH_BaseSetupDataAsset.h"
#include "Camera/CameraComponent.h"
#include "Component/CombatComponent/MH_PlayerCombatComponent.h"
#include "Core/MH_EnhanceInputComponent.h"
#include "Core/MH_GameplayTags.h"
#include "DataAsset/MH_DA_Input.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MH_ReCreate/Public/BPFuncLib/MH_BluePrintFuncLib.h"


// Sets default values
AMH_BasePlayerCharacter::AMH_BasePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	SpringArmComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	SpringArmComponent->TargetArmLength = 250.0f;
	CameraComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CombatComponent = CreateDefaultSubobject<UMH_PlayerCombatComponent>(TEXT("CombatComponent"));
	
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void AMH_BasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}


void AMH_BasePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	check(SetupDataAsset)
	ensureMsgf(!SetupDataAsset.IsNull(),TEXT("SetupDataAsset is nullptr"));
	
	UMH_BaseSetupDataAsset* LocalDA = SetupDataAsset.LoadSynchronous();
	if (LocalDA)
	{
		LocalDA->GiveAbilitiesToASC(AbilitySystemComponent);
	}
	
}

//这个的调用时机不需要由程序来控制，会由引擎来决定。然后这里只需要将能力设置为能本地执行并预测就好了。
//会将执行请求发给服务器的
void AMH_BasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	BindInput(PlayerInputComponent);
}


void AMH_BasePlayerCharacter::BindInput(UInputComponent* PlayerInputComponent)
{
	checkf(InputDA, TEXT("InputDA is nullptr"));
	
	ULocalPlayer* LocalPlayer =  GetController<APlayerController>()->GetLocalPlayer();
	check(LocalPlayer)
	
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(InputLocalPlayerSubsystem)
	InputLocalPlayerSubsystem->AddMappingContext(InputDA->InputMappingContext.Get(),0);
	
	UMH_EnhanceInputComponent* MH_InputComponent = CastChecked<UMH_EnhanceInputComponent>(PlayerInputComponent);
	MH_InputComponent->BindActionFromDA(InputDA,MH_GameplayTags::InputTag_Look, this, &ThisClass::Look, ETriggerEvent::Triggered);
	MH_InputComponent->BindActionFromDA(InputDA,MH_GameplayTags::InputTag_Move, this, &ThisClass::Move, ETriggerEvent::Triggered);
	MH_InputComponent->BindActionFromDA(InputDA,this,&ThisClass::AbilityPressed,&ThisClass::AbilityReleased);
}

void AMH_BasePlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookDir = Value.Get<FVector2D>();
	AddControllerYawInput(LookDir.X);
	AddControllerPitchInput(LookDir.Y);
}

void AMH_BasePlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveDir = Value.Get<FVector2D>();
	
	FVector Forward = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector Right = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(Forward, MoveDir.Y);
	AddMovementInput(Right, MoveDir.X);
	
}

void AMH_BasePlayerCharacter::AbilityPressed(FGameplayTag AbilityTag)
{
	check(AbilitySystemComponent)
	AbilitySystemComponent->OnAbilityInputPressed(AbilityTag);
}

void AMH_BasePlayerCharacter::AbilityReleased(FGameplayTag AbilityTag)
{
	check(AbilitySystemComponent)
	AbilitySystemComponent->OnAbilityInputReleased(AbilityTag);
}


