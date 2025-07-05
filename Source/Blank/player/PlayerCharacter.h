#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"


#include "PlayerCharacter.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimSequence;

UCLASS()
class BLANK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	void TakeDamage(float DamageAmount);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// Mesh and Animations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* IdleAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* RunAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* AttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* HitAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimSequence* DeathAnim;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	class UHealthBarWidget* HealthBarWidget;




	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	// State
	bool bIsAttacking = false;
	bool bIsDead = false;
	FTimerHandle AttackTimerHandle;

	// Movement
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpStarted(const FInputActionValue& Value);
	void JumpStopped(const FInputActionValue& Value);

	// Combat
	UFUNCTION()
	void Attack();

	UFUNCTION()
	void OnAttackFinished();

	UFUNCTION()
	bool IsDead() const { return CurrentHealth <= 0.f; }

	void StartRespawnCountdown();
	void RespawnPlayer();

	int32 RespawnCounter = 5;
	FTimerHandle RespawnCountdownTimerHandle;

	UFUNCTION()
	void UpdateHealthBar();

};
