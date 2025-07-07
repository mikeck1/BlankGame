#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AAIController;
class UAnimSequence;
class APlayerCharacter;

UCLASS()
class BLANK_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Damage Handling
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void ReceiveDamage(float DamageAmount);
	void PlayHit();
	void Die();
	void OnDeathAnimationFinished();

	// AI Combat
	void UpdateAI();
	void TryAttack();
	void ResetAttack();
	void PlayIdle();

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	// Animations
	UPROPERTY()
	UAnimSequence* HitAnim;

	UPROPERTY()
	UAnimSequence* DeathAnim;

	UPROPERTY()
	UAnimSequence* AttackAnim;

	UPROPERTY()
	UAnimSequence* IdleAnim;
	FTimerHandle HitAnimTimerHandle;

	// AI
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MovementRange = 1000.f;

	FTimerHandle AIUpdateTimerHandle;
	FTimerHandle AttackCooldownTimerHandle;

	bool bCanAttack = true;
	bool bIsDead = false;

	FTimerHandle DeathTimerHandle;
};
