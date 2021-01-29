// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Actors/TheCovenActor.h"
#include "Player/ShooterCharacter.h"
#include "TheCovenCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenCharacter : public AShooterCharacter
{
	GENERATED_UCLASS_BODY()

	virtual void Tick(float DeltaSeconds) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	bool bIsThirdPerson = false;

	virtual void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) override;

//////////////////////////////////////////////////////////////////////////
// Input handlers
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxUseDistance;

	ATheCovenActor* GetTheCovenActorInView();

	// returns actor we are currently holding "Use" on
	ATheCovenActor* CurrentlyUsed;

	/** player pressed use action */
	UFUNCTION(BlueprintNativeEvent, Category = PlayerAbility)
	void OnStartUse();

	UFUNCTION(WithValidation, Server, Reliable)
	void ServerOnStartUse();

	/** player released use action */
	UFUNCTION(BlueprintNativeEvent, Category = PlayerAbility)
	void OnStopUse();

	UFUNCTION(WithValidation, Server, Reliable)
	void ServerOnStopUse();

	/* True only in first frame when focused on new usable actor. */
	bool bHasNewFocus;

	/* Actor derived from UsableActor currently in center-view. */
	ATheCovenActor* FocusedUsableActor;
};
