#include "EnemyCharacter.h"
#include "player/PlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "AIController.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    // Load skeletal mesh
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/RPGHeroSquad/Mesh/Character/SK_DogPBR.SK_DogPBR"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
    }

    // Load animations
    static ConstructorHelpers::FObjectFinder<UAnimSequence> HitAnimObj(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_GetHit_RPGHero.Anim_GetHit_RPGHero"));
    if (HitAnimObj.Succeeded()) HitAnim = HitAnimObj.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAnimObj(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_Die_RPGHero.Anim_Die_RPGHero"));
    if (DeathAnimObj.Succeeded()) DeathAnim = DeathAnimObj.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnimObj(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_NormalAttack02_RPGHero.Anim_NormalAttack02_RPGHero"));
    if (AttackAnimObj.Succeeded()) AttackAnim = AttackAnimObj.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAnimObj(TEXT("/Game/RPGHeroSquad/Animation/RPGHero/Anim_Idle_RPGHero.Anim_Idle_RPGHero"));
    if (IdleAnimObj.Succeeded()) IdleAnim = IdleAnimObj.Object;

    Tags.Add(FName("Enemy"));

    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;

    AttackRange = 150.f;
    MovementRange = 1000.f;
    bCanAttack = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Your AI update or other logic here
    UpdateAI();
}


void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    GetWorldTimerManager().SetTimer(AIUpdateTimerHandle, this, &AEnemyCharacter::UpdateAI, 0.5f, true);

    if (IdleAnim)
    {
        GetMesh()->PlayAnimation(IdleAnim, true);
    }
}

void AEnemyCharacter::UpdateAI()
{
    if (!PlayerCharacter || CurrentHealth <= 0.f) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

    if (Distance > AttackRange && Distance < MovementRange)
    {
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            AIController->MoveToActor(PlayerCharacter);
        }
    }
    else if (Distance <= AttackRange)
    {
        TryAttack();
    }
}

void AEnemyCharacter::TryAttack()
{
    if (!bCanAttack || CurrentHealth <= 0.f || !PlayerCharacter) return;

    bCanAttack = false;

    if (AttackAnim)
    {
        GetMesh()->PlayAnimation(AttackAnim, false);
    }

    // Apply damage to player
    PlayerCharacter->TakeDamage(20.f);

    // Cooldown before next attack
    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacter::ResetAttack, 2.0f, false);
}

void AEnemyCharacter::ResetAttack()
{
    bCanAttack = true;
    GetMesh()->PlayAnimation(IdleAnim, true); // true = loop
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    ReceiveDamage(DamageAmount);
    return DamageAmount;
}

void AEnemyCharacter::ReceiveDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("Enemy took damage: %f, health left: %f"), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    else
    {
        PlayHit();
    }
}

void AEnemyCharacter::PlayHit()
{
    if (HitAnim)
    {
        GetMesh()->PlayAnimation(HitAnim, false);

        // Return to idle after hit finishes
        GetWorldTimerManager().SetTimer(HitAnimTimerHandle, this, &AEnemyCharacter::PlayIdle, HitAnim->GetPlayLength(), false);
    }
}

void AEnemyCharacter::PlayIdle()
{
    if (bIsDead || !IdleAnim) return;

    GetMesh()->PlayAnimation(IdleAnim, true);
}

void AEnemyCharacter::Die()
{
    bIsDead = true;

    if (DeathAnim)
    {
        GetMesh()->PlayAnimation(DeathAnim, false);
        SetActorEnableCollision(false);
        GetCharacterMovement()->DisableMovement();

        GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyCharacter::OnDeathAnimationFinished, DeathAnim->GetPlayLength(), false);
    }
    else
    {
        SetActorEnableCollision(false);
        GetCharacterMovement()->DisableMovement();
        GetMesh()->SetVisibility(false);
    }
}


void AEnemyCharacter::OnDeathAnimationFinished()
{
    //Destroy();
}
