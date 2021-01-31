// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "UI/TheCovenHUD.h"
#include "Player/TheCovenPlayerController.h"
#include "Online/TheCovenPlayerState.h"
#include "Player/TheCovenCharacter.h"
#include "TheCoven/TheCovenGameMode.h"

// Witch team num == 0
// Ghost team num == 1

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

	PlayerControllerClass = ATheCovenPlayerController::StaticClass();

	bAllowBots = false;
	bDelayedStart = true;
	MinRespawnDelay = 2000.0f;
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

int32 ATheCovenGameMode::ChooseTeam(AShooterPlayerState* ForPlayerState) const
{
	// Team 0 are the Witches, Team 1 is the Ghost/Wraith

	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		AShooterPlayerState const* const TestPlayerState = Cast<AShooterPlayerState>(GameState->PlayerArray[i]);

		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNum()))
		{
			TeamBalance[TestPlayerState->GetTeamNum()]++;
		}
	}

	const int32 RandomBestTeam = FMath::RandHelper(TeamBalance.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("RandomBestTeam: %lld"), RandomBestTeam));
	if (RandomBestTeam == 1) {
		if (TeamBalance[1] == 0)
		{
			// Assign to Ghost
			return 1;
		}
	}

	if (TeamBalance[1] == 0 && TeamBalance[0] == 1)
	{
		// Need at least 1 Ghost
		return 1;
	}

	// Assign to Witch
	return 0;
}

void ATheCovenGameMode::DefaultTimer()
{
	// don't update timers for Play In Editor mode, it's not real match
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
	if (GetWorld()->IsPlayInEditor())
	{
		/*
		if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
		{
			MyGameState->RemainingTime--;
		}
		*/
		// start match if necessary.
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		//return;
	}

	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("GameOver"));

			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Waiting Post Match"));
				RestartGame();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();

				// Send end round events
				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
				{
					AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*It);

					if (PlayerController && MyGameState)
					{
						AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>((*It)->PlayerState);
						const bool bIsWinner = IsWinner(PlayerState);

						PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
					}
				}
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Start New Match"));
				StartMatch();
			}
		}
	}
}

void ATheCovenGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	Super::Killed(Killer, KilledPlayer, KilledPawn, DamageType);

	AShooterGameState* MyGameState = CastChecked<AShooterGameState>(GameState);
	int32 TeamDeaths = 0;
	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		AShooterPlayerState* PS = (AShooterPlayerState*)MyGameState->PlayerArray[i];
		if (!PS) {
			return;
		}
		const int32 PlayerDeaths = PS->GetDeaths();
		
		if (PS->GetTeamNum() == 1) {
			if (PlayerDeaths >= 1)
			{
				MyGameState->RemainingTime = 1;
			}
		}
		else {
			TeamDeaths++;
			if (TeamDeaths >=(MyGameState->PlayerArray.Num() - 1)) {
				MyGameState->RemainingTime = 1;
			}
		}
	}
}

void ATheCovenGameMode::DetermineMatchWinner()
{
	AShooterGameState* MyGameState = CastChecked<AShooterGameState>(GameState);
	int32 BestScore = MIN_uint32;
	int32 BestTeam = -1;
	int32 NumBestTeams = 1;

	int32 TeamDeaths = 0;
	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		AShooterPlayerState* PS = (AShooterPlayerState*)MyGameState->PlayerArray[i];
		if (!PS) {
			return;
		}
		const int32 PlayerDeaths = PS->GetDeaths();

		if (PS->GetTeamNum() == 1) {
			if (PlayerDeaths >= 1)
			{
				WinnerTeam = 0;
			}
		}
		else {
			TeamDeaths++;
			if (TeamDeaths >= (MyGameState->PlayerArray.Num() - 1)) {
				WinnerTeam = 1;
			}
		}
	}

	//WinnerTeam = (NumBestTeams == 1) ? BestTeam : NumTeams;

	if (WinnerTeam == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("DetermineMatchWinner Witch"));
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("DetermineMatchWinner Ghost"));
	}
}

bool ATheCovenGameMode::IsWinner(AShooterPlayerState* PlayerState) const
{
	if (PlayerState->GetTeamNum() == WinnerTeam)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Winner!"));
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Loser"));
	}
	return PlayerState && !PlayerState->IsQuitter() && PlayerState->GetTeamNum() == WinnerTeam;
}