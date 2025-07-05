#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h" // Needed for UProgressBar
#include "HealthBarWidget.generated.h"

UCLASS()
class BLANK_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// This will be bound to your UMG progress bar named ProgressBar_HealthBar
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_HealthBar;

	// Call this to update the visual health bar
	UFUNCTION(BlueprintCallable)
	void SetHealthPercent(float Percent);
};
