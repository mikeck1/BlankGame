#include "../player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimSingleNodeInstance.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "../HealthBarWidget.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Load Input Mapping Context and Actions
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(TEXT("/Game/input_actions/IMC_DefaultContext"));
	if (IMC.Succeeded()) DefaultMappingContext = IMC.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveInput(TEXT("/Game/input_actions/Actions/IA_Move"));
	if (MoveInput.Succeeded()) MoveAction = MoveInput.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> LookInput(TEXT("/Game/input_actions/Actions/IA_Look"));
	if (LookInput.Succeeded()) LookAction = LookInput.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpInput(TEXT("/Game/input_actions/Actions/IA_Jump"));
	if (JumpInput.Succeeded()) JumpAction = JumpInput.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackInput(TEXT("/Game/input_actions/Actions/IA_Attack"));
	if (AttackInput.Succeeded()) AttackAction = AttackInput.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarBPClass(TEXT("/Game/UI/WBP_HealthBar_Custom"));
	if (HealthBarBPClass.Succeeded())
	{
		HealthBarWidgetClass = HealthBarBPClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find WBP_HealthBar class."));
	}



	// Camera setup
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Movement setup
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Mesh setup
	CharacterMesh = GetMesh();
	CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// Load Animations
	static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAnimFinder(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_Idle_RPGHero.Anim_Idle_RPGHero"));
	if (IdleAnimFinder.Succeeded()) IdleAnim = IdleAnimFinder.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> RunAnimFinder(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/InPlace/Anim_Run_IP_RPGHero.Anim_Run_IP_RPGHero"));
	if (RunAnimFinder.Succeeded()) RunAnim = RunAnimFinder.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnimFinder(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_NormalAttack01_RPGHero.Anim_NormalAttack01_RPGHero"));
	if (AttackAnimFinder.Succeeded()) AttackAnim = AttackAnimFinder.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAnimFinder(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_Die_RPGHero.Anim_Die_RPGHero"));
	if (DeathAnimFinder.Succeeded()) DeathAnim = DeathAnimFinder.Object;


	// Load Skeletal Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/RPGHeroSquad/Mesh/Character/SK_RPGHeroPolyart.SK_RPGHeroPolyart"));
	if (MeshFinder.Succeeded())
	{
		CharacterMesh->SetSkeletalMesh(MeshFinder.Object);
		CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		if (IdleAnim) CharacterMesh->PlayAnimation(IdleAnim, true);
	}

	CurrentHealth = MaxHealth;
}



void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead || bIsAttacking)
	{
		// Skip changing animations if dead or attacking
		return;
	}

	float Speed = GetVelocity().Size2D();

	if (Speed > 10.f && RunAnim)
	{
		if (UAnimSingleNodeInstance* Node = CharacterMesh->GetSingleNodeInstance())
		{
			if (Node->GetCurrentAsset() != RunAnim)
			{
				CharacterMesh->PlayAnimation(RunAnim, true);
			}
		}
	}
	else if (Speed <= 10.f && IdleAnim)
	{
		if (UAnimSingleNodeInstance* Node = CharacterMesh->GetSingleNodeInstance())
		{
			if (Node->GetCurrentAsset() != IdleAnim)
			{
				CharacterMesh->PlayAnimation(IdleAnim, true);
			}
		}
	}
}





// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter is active and spawned"));

	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UHealthBarWidget>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Created widget: %s (Class: %s)"),
				*HealthBarWidget->GetName(),
				*HealthBarWidget->GetClass()->GetName());

			HealthBarWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HealthBarWidget instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBarWidgetClass is null!"));
	}



	APlayerController* PC = Cast<APlayerController>(GetController());
	UE_LOG(LogTemp, Warning, TEXT(">> PlayerController: %s"), PC ? *PC->GetName() : TEXT("nullptr"));

	if (PC && PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter Mapping context added"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("APlayerCharacter InputMappingContext is null!"));
			}
		}
	}
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpStopped);
		Input->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	if (Controller && Input.SizeSquared() > 0.f)
	{
		const FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, Input.Y);
		AddMovementInput(Right, Input.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D Look = Value.Get<FVector2D>();
	AddControllerYawInput(Look.X);
	AddControllerPitchInput(-Look.Y);  // Invert Y
}

void APlayerCharacter::JumpStarted(const FInputActionValue& Value)
{
	if (bIsDead) return;

	Jump();
}

void APlayerCharacter::JumpStopped(const FInputActionValue& Value)
{
	if (bIsDead) return;

	StopJumping();
}

void APlayerCharacter::Attack()
{
	if (bIsDead || bIsAttacking || !AttackAnim || !FollowCamera) return;

	bIsAttacking = true;
	CharacterMesh->PlayAnimation(AttackAnim, false);

	// Use character's location + forward vector, not camera location, for trace origin:
	FVector Start = GetActorLocation() + FVector(0, 0, 50); // 50 units up to roughly chest height
	FVector Forward = GetActorForwardVector();
	FVector End = Start + (Forward * 300.f);  // Increased range for testing

	// Debug draw line and capsule to visualize trace
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 2.f);
	DrawDebugCapsule(GetWorld(), End, 10.f, 10.f, FQuat::Identity, FColor::Blue, false, 2.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack trace hit: %s"), *Hit.GetActor()->GetName());

		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag("Enemy"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit enemy! Applying damage."));
			UGameplayStatics::ApplyDamage(HitActor, 25.f, GetController(), this, nullptr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack trace missed."));
	}

	float AnimDuration = AttackAnim->GetPlayLength();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlayerCharacter::OnAttackFinished, AnimDuration, false);
}


void APlayerCharacter::OnAttackFinished()
{
	bIsAttacking = false;

	// Return to Idle or Run based on speed immediately or on next Tick
	float Speed = GetVelocity().Size2D();
	if (Speed > 10.f)
		CharacterMesh->PlayAnimation(RunAnim, true);
	else
		CharacterMesh->PlayAnimation(IdleAnim, true);
}


void APlayerCharacter::TakeDamage(float DamageAmount)
{
	if (IsDead() || bIsDead) return;

	CurrentHealth -= DamageAmount;
	UpdateHealthBar();

	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;
		bIsDead = true;

		UE_LOG(LogTemp, Warning, TEXT("Player died."));

		bIsAttacking = false;
		if (DeathAnim)
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->PlayAnimation(DeathAnim, false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DeathAnim is null! Please set the DeathAnim asset."));
		}


		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC) PC->DisableInput(PC);

		// Start respawn countdown with a slight delay
		GetWorldTimerManager().SetTimer(RespawnCountdownTimerHandle, this, &APlayerCharacter::StartRespawnCountdown, 0.5f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player took damage. Health: %f"), CurrentHealth);

		if (HitAnim)
		{
			CharacterMesh->PlayAnimation(HitAnim, false);
		}
	}
}


void APlayerCharacter::StartRespawnCountdown()
{
	RespawnCounter = 5;
	GetWorldTimerManager().SetTimer(RespawnCountdownTimerHandle, this, &APlayerCharacter::RespawnPlayer, 1.0f, true);
}

void APlayerCharacter::RespawnPlayer()
{
	if (RespawnCounter > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
			FString::Printf(TEXT("Respawning in %d..."), RespawnCounter));

		RespawnCounter--;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RespawnCountdownTimerHandle);

		CurrentHealth = MaxHealth;
		bIsDead = false;  // <--- important to reset this here!

		SetActorLocation(FVector(-20656.656936, -31919.160895, 1916.80985));

		CharacterMesh->PlayAnimation(IdleAnim, true);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC) PC->EnableInput(PC);
	}
}

void APlayerCharacter::UpdateHealthBar()
{
	if (!HealthBarWidget) return;

	// Call Blueprint function from the widget
	float Percent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);
	HealthBarWidget->SetHealthPercent(Percent);
}
