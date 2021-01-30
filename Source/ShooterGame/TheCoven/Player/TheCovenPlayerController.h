// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterPlayerController.h"
#include "TheCovenPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenPlayerController : public AShooterPlayerController
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHidden(bool bIsHidden);

	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetHidden(bool bIsHidden);
};
