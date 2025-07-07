// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Sword	UMETA(DisplayName = "Sword"),
	Shield	UMETA(DisplayName = "Shield"),
	Potion	UMETA(DisplayName = "Potion"),
	Other	UMETA(DisplayName = "Other")
};

UCLASS(Blueprintable)
class BLANK_API UItemBase : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Damage = 0.f;


	// Constructor
	UItemBase();

	// Logic hook if needed
	virtual void Use(class APlayerCharacter* Character);

	static UItemBase* CreateSword(UObject* Outer);
	static UItemBase* CreateShield(UObject* Outer);
};