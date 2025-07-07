// Fill out your copyright notice in the Description page of Project Settings.

#include "DroppedItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "../player/PlayerCharacter.h"
#include "../Items/ItemBase.h"

ADroppedItemActor::ADroppedItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetLinearDamping(5.f);
	MeshComponent->SetAngularDamping(5.f);


	// Bind overlap
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ADroppedItemActor::OnOverlapBegin);
}

void ADroppedItemActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADroppedItemActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player && ItemData)
	{
		Player->Inventory.Add(ItemData);
		Player->EquipCurrentItem();
		Destroy();
	}
}
