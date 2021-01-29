// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ShooterCharacter.h"
#include "TheCovenCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenCharacter : public AShooterCharacter
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	bool bIsThirdPerson = false;

	virtual void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) override;
};
