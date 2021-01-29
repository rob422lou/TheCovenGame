// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/ShooterGame_TeamDeathMatch.h"
#include "TheCovenGameMode.generated.h"

class ATheCovenPlayerController;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API ATheCovenGameMode : public AShooterGame_TeamDeathMatch
{
	GENERATED_UCLASS_BODY()

	ATheCovenGameMode();
	
public:
	/** The witch pawn class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APawn> WitchPawnClass;
	/** The ghost pawn class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APawn> GhostPawnClass;

	virtual void StartPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	void UpdatePlayerPawns(ATheCovenPlayerController* PC);
};
