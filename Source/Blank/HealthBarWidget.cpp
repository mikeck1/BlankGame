// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthBarWidget.h"

void UHealthBarWidget::SetHealthPercent(float Percent)
{
	if (ProgressBar_HealthBar)
	{
		ProgressBar_HealthBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
		UE_LOG(LogTemp, Warning, TEXT("SetHealthPercent called: %f"), Percent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProgressBar_HealthBar is null!"));

	}
}
