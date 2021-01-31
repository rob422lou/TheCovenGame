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

	// Overridden from GamemodeBase class
	virtual void RestartPlayer(AController* NewPlayer) override;

	void UpdatePlayerPawns(ATheCovenPlayerController* PC);

////////////////////////////////////////////////////////////////
// ShooterGame overrides
public:
	virtual int32 ChooseTeam(AShooterPlayerState* ForPlayerState) const;

	/** update remaining time */
	virtual void DefaultTimer() override;

	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** best player */
	UPROPERTY(transient)
	AShooterPlayerState* WinnerPlayerState;

	/** check who won */
	virtual void DetermineMatchWinner() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(AShooterPlayerState* PlayerState) const override;
};
