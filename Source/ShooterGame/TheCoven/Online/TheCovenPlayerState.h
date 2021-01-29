// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/ShooterPlayerState.h"
#include "TheCovenPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenPlayerState : public AShooterPlayerState
{
	GENERATED_UCLASS_BODY()
	
public:
	void UpdatePlayerPawn();

/////////////////////////////////////////////////////////////////////////////
// ShooterPlayerState Overrides
public:
	virtual void ClientInitialize(class AController* InController) override;

	virtual void SetTeamNum(int32 NewTeamNumber) override;

	virtual void OnRep_TeamColor() override;
};
