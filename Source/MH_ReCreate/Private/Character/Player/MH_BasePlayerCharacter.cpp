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
#include "Component/MH_CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Locomotion/MH_DA_GaitLocomotionParams.h"
#include "Locomotion/Interface/MH_PlayerAnimInterface.h"
#include "MH_ReCreate/Public/BPFuncLib/MH_BluePrintFuncLib.h"


// Sets default values
AMH_BasePlayerCharacter::AMH_BasePlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMH_CharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void AMH_BasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 如果蓝图类默认对象清空了原生组件指针，则重新查找组件。
	if (!IsValid(CombatComponent))
	{
		CombatComponent = FindComponentByClass<UMH_PlayerCombatComponent>();
	}

	ensureMsgf(IsValid(CombatComponent), TEXT("%s has no UMH_PlayerCombatComponent"), *GetName());
	
	//更新默认姿态运动参数
	OnGaitTypeChange(GaitType);
}

void AMH_BasePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
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


void AMH_BasePlayerCharacter::OnGaitTypeChange(const FGaitType InGaitType)
{
	SetBaseGaitType(InGaitType);
}

bool AMH_BasePlayerCharacter::SetBaseGaitType(const FGaitType InBaseGaitType)
{
	return ApplyLocomotionState(InBaseGaitType, MoveState);
}

bool AMH_BasePlayerCharacter::SetRunState(const bool bInRunning)
{
	return ApplyLocomotionState(BaseGaitType, bInRunning ? EMoveState::Run : EMoveState::None);
}

bool AMH_BasePlayerCharacter::ResetMovementStateForJump()
{
	UnCrouch();
	return ApplyLocomotionState(FGaitType::Walk, EMoveState::None);
}

bool AMH_BasePlayerCharacter::ApplyLocomotionState(
	const FGaitType InBaseGaitType,
	const EMoveState InMoveState)
{
	if (!IsValid(LocomotionParams))
	{
		UE_LOG(LogMH, Error, TEXT("%s: LocomotionParams is not configured"), *GetName());
		return false;
	}

	const FGaitMoveStateParameters* GaitParameters = LocomotionParams->GaitParams.FindByPredicate(
		[InBaseGaitType](const FGaitMoveStateParameters& Entry)
		{
			return Entry.GaitType == InBaseGaitType;
		});
	const FLocomotionParameters* Parameters = GaitParameters
		? GaitParameters->MoveStateParams.Find(InMoveState)
		: nullptr;
	if (!Parameters)
	{
		UE_LOG(LogMH, Error, TEXT("%s: LocomotionParams has no gait/move-state entry for gait %d and move state %d"),
			*GetName(), static_cast<uint8>(InBaseGaitType), static_cast<uint8>(InMoveState));
		return false;
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!IsValid(MovementComponent))
	{
		UE_LOG(LogMH, Error, TEXT("%s: CharacterMovementComponent is invalid"), *GetName());
		return false;
	}

	// CMC 在蹲下时读取 MaxWalkSpeedCrouched，不能只修改 MaxWalkSpeed。
	if (InBaseGaitType == FGaitType::Crouch)
	{
		MovementComponent->MaxWalkSpeedCrouched = Parameters->MaxWalkSpeed;
	}
	else
	{
		MovementComponent->MaxWalkSpeed = Parameters->MaxWalkSpeed;
	}
	MovementComponent->MaxAcceleration = Parameters->MaxAcceleration;
	MovementComponent->BrakingDecelerationWalking = Parameters->BrakingDeceleration;
	MovementComponent->BrakingFrictionFactor = Parameters->BrakingFrictionFactor;
	MovementComponent->BrakingFriction = Parameters->BrakingFriction;
	MovementComponent->bUseSeparateBrakingFriction = Parameters->bUseSeparateBrakingFriction;

	BaseGaitType = InBaseGaitType;
	MoveState = InMoveState;
	bIsRunning = MoveState == EMoveState::Run;
	GaitType = BaseGaitType;

	if (USkeletalMeshComponent* MeshComponent = GetMesh();
		IMH_PlayerAnimInterface* AnimInterface = MeshComponent
			? Cast<IMH_PlayerAnimInterface>(MeshComponent->GetAnimInstance())
			: nullptr)
	{
		AnimInterface->SetGaitType(GaitType);
		AnimInterface->SetMoveState(MoveState);
	}

	return true;
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

UMH_PlayerCombatComponent* AMH_BasePlayerCharacter::GetMHCombatComponent() const
{
	if (IsValid(CombatComponent))
	{
		return CombatComponent;
	}

	return FindComponentByClass<UMH_PlayerCombatComponent>();
}

UMH_CharacterMovementComponent* AMH_BasePlayerCharacter::GetMHCharacterMovementComponent() const
{
	return Cast<UMH_CharacterMovementComponent>(GetCharacterMovement());
}

bool AMH_BasePlayerCharacter::IsSliding() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent && MovementComponent->IsSliding();
}

bool AMH_BasePlayerCharacter::CanSlide() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent && MovementComponent->CanSlide();
}

float AMH_BasePlayerCharacter::GetGroundSlopeAngle() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent ? MovementComponent->GroundSlopeAngle : 0.0f;
}

void AMH_BasePlayerCharacter::SetAirborneControlEnabled(const bool bEnabled)
{
	if (UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent())
	{
		MovementComponent->SetAirborneControlEnabled(bEnabled);
	}
}

void AMH_BasePlayerCharacter::SetAirborneMovementEnabled(const bool bEnabled)
{
	if (UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent())
	{
		MovementComponent->SetAirborneMovementEnabled(bEnabled);
	}
}

void AMH_BasePlayerCharacter::SetAirborneRotationEnabled(const bool bEnabled)
{
	if (UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent())
	{
		MovementComponent->SetAirborneRotationEnabled(bEnabled);
	}
}

bool AMH_BasePlayerCharacter::IsAirborneControlEnabled() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent && MovementComponent->IsAirborneControlEnabled();
}

bool AMH_BasePlayerCharacter::IsAirborneMovementEnabled() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent && MovementComponent->IsAirborneMovementEnabled();
}

bool AMH_BasePlayerCharacter::IsAirborneRotationEnabled() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent && MovementComponent->IsAirborneRotationEnabled();
}

ECrouchInputAction AMH_BasePlayerCharacter::GetPredictedCrouchInputAction() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return IsSliding() || (MovementComponent && MovementComponent->CanPerformLedgeJump())
		? ECrouchInputAction::Jump
		: ECrouchInputAction::Crouch;
}

ECrouchInputReason AMH_BasePlayerCharacter::GetPredictedCrouchInputReason() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	if (IsSliding())
	{
		return ECrouchInputReason::Sliding;
	}
	if (MovementComponent && MovementComponent->CanPerformLedgeJump())
	{
		return ECrouchInputReason::Ledge;
	}
	return ECrouchInputReason::NormalGround;
}

float AMH_BasePlayerCharacter::GetSlideSpeed() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent ? MovementComponent->SlideSpeed : 0.0f;
}

FVector AMH_BasePlayerCharacter::GetSlideDirection() const
{
	const UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	return MovementComponent ? MovementComponent->SlideDirection : FVector::ZeroVector;
}

bool AMH_BasePlayerCharacter::TryGetForwardLedgeJumpVelocity(FVector& OutJumpVelocity)
{
	ResetLedgeJumpState();
	OutJumpVelocity = FVector::ZeroVector;

	UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	const UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();
	if (!MovementComponent || !CharacterCapsule || MovementComponent->IsFalling())
	{
		return false;
	}

	FHitResult LedgeHit;
	float DropHeight = 0.0f;
	if (!FindForwardLedgeDrop(LedgeHit, DropHeight))
	{
		return false;
	}

	const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal2D();
	if (ForwardDirection.IsNearlyZero())
	{
		return false;
	}

	const float CapsuleHeight = CharacterCapsule->GetScaledCapsuleHalfHeight() * 2.0f;
	if (DropHeight < CapsuleHeight)
	{
		return false;
	}

	OutJumpVelocity = ForwardDirection * LedgeJumpForwardSpeed
		+ FVector::UpVector * LedgeJumpUpSpeed;
	LedgeDropHeight = DropHeight;
	LedgeJumpVelocity = OutJumpVelocity;
	bLedgeJumpAvailable = true;
	return true;
}

bool AMH_BasePlayerCharacter::TryPerformContextualJump()
{
	ResetLedgeJumpState();

	UMH_CharacterMovementComponent* MovementComponent = GetMHCharacterMovementComponent();
	if (!MovementComponent || MovementComponent->IsFalling())
	{
		return false;
	}

	// 蹲姿下本次输入只允许解除蹲姿，不能直接进入跳跃逻辑。
	if (BaseGaitType == FGaitType::Crouch || IsCrouched())
	{
		return false;
	}

	// CMC 已经在移动更新中维护滑行状态，输入事件只需读取并执行跳跃。
	MovementComponent->RefreshSurfaceState();
	if (MovementComponent->IsSliding())
	{
		const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal2D();
		if (ForwardDirection.IsNearlyZero())
		{
			return false;
		}
		if (!ResetMovementStateForJump())
		{
			return false;
		}

		LedgeJumpVelocity = ForwardDirection * LedgeJumpForwardSpeed
			+ FVector::UpVector * LedgeJumpUpSpeed;
		LaunchCharacter(LedgeJumpVelocity, true, true);
		return true;
	}

	if (!MovementComponent->CanPerformLedgeJump())
	{
		return false;
	}

	const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal2D();
	if (ForwardDirection.IsNearlyZero())
	{
		return false;
	}
	if (!ResetMovementStateForJump())
	{
		return false;
	}
	LedgeDropHeight = MovementComponent->LedgeDropHeight;
	LedgeJumpVelocity = ForwardDirection * LedgeJumpForwardSpeed
		+ FVector::UpVector * LedgeJumpUpSpeed;
	bLedgeJumpAvailable = true;
	LaunchCharacter(LedgeJumpVelocity, true, true);
	return true;
}

bool AMH_BasePlayerCharacter::FindForwardLedgeDrop(
	FHitResult& OutHitResult,
	float& OutDropHeight) const
{
	OutHitResult = FHitResult();
	OutDropHeight = 0.0f;

	const UWorld* World = GetWorld();
	const UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();
	if (!World || !CharacterCapsule)
	{
		return false;
	}

	const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal2D();
	if (ForwardDirection.IsNearlyZero())
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MHForwardLedge), false, this);
	QueryParams.AddIgnoredActor(this);

	// 先检查前方是否有墙体，避免把墙前方的地面误判为悬崖落点。
	const FVector WallStart = GetActorLocation();
	const FVector WallEnd = WallStart + ForwardDirection * LedgeProbeDistance;
	if (World->LineTraceTestByChannel(WallStart, WallEnd, LedgeTraceChannel, QueryParams))
	{
		return false;
	}

	const float CapsuleHalfHeight = CharacterCapsule->GetScaledCapsuleHalfHeight();
	const FVector CharacterBase = GetActorLocation() - FVector::UpVector * CapsuleHalfHeight;
	const FVector ProbeLocation = CharacterBase + ForwardDirection * LedgeProbeDistance;

	// 从胶囊体顶部向世界下限检测，不设置可配置的最大悬崖落差。
	float TraceEndZ = World->GetWorldSettings()
		? World->GetWorldSettings()->KillZ
		: ProbeLocation.Z - 100000.0f;
	if (TraceEndZ >= ProbeLocation.Z)
	{
		TraceEndZ = ProbeLocation.Z - 100000.0f;
	}

	const FVector TraceStart(
		ProbeLocation.X,
		ProbeLocation.Y,
		ProbeLocation.Z + CapsuleHalfHeight * 2.0f);
	const FVector TraceEnd(ProbeLocation.X, ProbeLocation.Y, TraceEndZ);
	if (!World->LineTraceSingleByChannel(
		OutHitResult,
		TraceStart,
		TraceEnd,
		LedgeTraceChannel,
		QueryParams))
	{
		return false;
	}

	OutDropHeight = CharacterBase.Z - OutHitResult.ImpactPoint.Z;
	return OutDropHeight > 0.0f;
}

void AMH_BasePlayerCharacter::ResetLedgeJumpState()
{
	bLedgeJumpAvailable = false;
	LedgeDropHeight = 0.0f;
	LedgeJumpVelocity = FVector::ZeroVector;
}
