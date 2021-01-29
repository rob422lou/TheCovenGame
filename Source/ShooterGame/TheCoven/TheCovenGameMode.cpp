// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "UI/TheCovenHUD.h"
#include "Player/TheCovenPlayerController.h"
#include "Online/TheCovenPlayerState.h"
#include "Player/TheCovenCharacter.h"
#include "TheCoven/TheCovenGameMode.h"

ATheCovenGameMode::ATheCovenGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/TheCovenDefaultPlayerPawn"));
	DefaultPawnClass = PlayerPawnOb.Class;

	static ConstructorHelpers::FClassFinder<APawn> GhostPlayerPawnOb(TEXT("/Game/Blueprints/Pawns/TheCovenGhostPlayerPawn"));
	GhostPawnClass = GhostPlayerPawnOb.Class;

	static ConstructorHelpers::FClassFinder<APawn> WitchPlayerPawnOb(TEXT("/Game/Blueprints/Pawns/TheCovenWitchPlayerPawn"));
	WitchPawnClass = WitchPlayerPawnOb.Class;

	//GameStateClass = ATheCovenGameState::StaticClass();
	HUDClass = ATheCovenHUD::StaticClass();

	bAllowBots = false;
	MinRespawnDelay = 2.0f;
}

void ATheCovenGameMode::StartPlay()
{
	Super::StartPlay();

	//UpdatePlayerPawns();
}

void ATheCovenGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (NewPlayer->GetPawn() != NULL) {
		UpdatePlayerPawns((ATheCovenPlayerController*)NewPlayer);
	}
}

void ATheCovenGameMode::UpdatePlayerPawns(ATheCovenPlayerController* PC)
{
	if (!PC) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("UpdatePlayerPawns - No PC"));
		return;
	}

	if (PC->GetPawn() == NULL) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("UpdatePlayerPawn() Pawn == NULL"));
		return;
	}

	if (((ATheCovenPlayerState*)PC->PlayerState)->GetTeamNum() == 0) {
		//Witch	
		FActorSpawnParameters SpawnParams;
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(WitchPawnClass, PC->GetPawn()->GetActorLocation(), PC->GetPawn()->GetActorRotation(), SpawnParams);
		ATheCovenCharacter* DestroyPawn = (ATheCovenCharacter*)PC->GetPawn();
		PC->Possess(NewCharacter);

		if (DestroyPawn)
		{
			DestroyPawn->Destroy();
		}

	}
	else {
		//Ghost	
		FActorSpawnParameters SpawnParams;
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(GhostPawnClass, PC->GetPawn()->GetActorLocation(), PC->GetPawn()->GetActorRotation(), SpawnParams);
		ATheCovenCharacter* DestroyPawn = (ATheCovenCharacter*)PC->GetPawn();
		PC->Possess(NewCharacter);

		if (DestroyPawn)
		{
			DestroyPawn->Destroy();
		}

	}
}