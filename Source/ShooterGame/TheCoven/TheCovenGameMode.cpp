// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "UI/TheCovenHUD.h"
#include "TheCoven/TheCovenGameMode.h"

ATheCovenGameMode::ATheCovenGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/TheCovenDefaultPlayerPawn"));
	DefaultPawnClass = PlayerPawnOb.Class;
	//GameStateClass = ATheCovenGameState::StaticClass();
	HUDClass = ATheCovenHUD::StaticClass();

	bAllowBots = false;
	MinRespawnDelay = 2.0f;
}