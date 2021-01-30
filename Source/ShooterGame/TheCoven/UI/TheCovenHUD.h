// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShooterHUD.h"
#include "TheCovenHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenHUD : public AShooterHUD
{
	GENERATED_BODY()
	
	virtual void DrawHUD() override;

	/** Temporary helper for drawing text-in-a-box. */
	void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);

	virtual void DrawHealth();
	void DrawTimer();

	virtual bool ShowScoreboard(bool bEnable, bool bFocus = false);
};
