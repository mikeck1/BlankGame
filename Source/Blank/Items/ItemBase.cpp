// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "../player/PlayerCharacter.h"

UItemBase::UItemBase()
{
    ItemName = FText::FromString("New Item");
    ItemType = EItemType::Other;
    MaxStackSize = 1;
    Mesh = nullptr;
    Icon = nullptr;
    Description = FText::FromString("Description");

    Damage = 25.f; // Default damage (e.g. fists)
}


void UItemBase::Use(APlayerCharacter* Character)
{
	// Default use behavior
	UE_LOG(LogTemp, Warning, TEXT("Using item: %s"), *ItemName.ToString());
}

UItemBase* UItemBase::CreateSword(UObject* Outer)
{
	UItemBase* Sword = NewObject<UItemBase>(Outer);
	Sword->ItemName = FText::FromString("RPG Sword");
	Sword->ItemType = EItemType::Sword;
	Sword->Damage = 100.f;
	Sword->Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/RPGHeroSquad/Mesh/Weapon/SM_RPGHero_Sword01_Polyart.SM_RPGHero_Sword01_Polyart"));
	// Optional: add Icon or Description
	return Sword;
}

UItemBase* UItemBase::CreateShield(UObject* Outer)
{
	UItemBase* Shield = NewObject<UItemBase>(Outer);
	Shield->ItemName = FText::FromString("Basic Shield");
	Shield->ItemType = EItemType::Shield;
	Shield->Damage = 0.f; // no attack damage
	Shield->Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/RPGHeroSquad/Mesh/Weapon/SM_RPGHero_Shield01_Polyart.SM_RPGHero_Shield01_Polyart"));
	return Shield;
}
