// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TheCoven/Player/TheCovenCharacter.h"
#include "TheCoven/Player/TheCovenPlayerController.h"
#include "TheCoven/Online/TheCovenPlayerState.h"

ATheCovenPlayerState::ATheCovenPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ATheCovenPlayerState::UpdatePlayerPawn()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UpdatePlayerPawn()"));
	
	ATheCovenPlayerController* PC = (ATheCovenPlayerController*)UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) {
		return;
	}
	
	if (GetTeamNum() == 0) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Witch1"));

		//Witch	
		auto* cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Game/Blueprints/Pawns/TheCovenGhostPlayerPawn"));
		if (!cls) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Witch1.5"));
			return;
		}
		UBlueprint* bp = Cast<UBlueprint>(cls);
		if (!bp) {
			return;
		}
		AShooterPlayerController* PC = (AShooterPlayerController*)UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PC) {
			return;
		}
		PC->UnPossess();
		TSubclassOf<class UObject> MyItemBlueprint;
		MyItemBlueprint = (UClass*)bp->GeneratedClass;

		FActorSpawnParameters SpawnParams;
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(MyItemBlueprint, { 0, 0, 0 }, { 0, 0, 0 }, SpawnParams);
		PC->Possess(NewCharacter);
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Witch2"));
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ghost11"));
		//Ghost
		auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'Content/Blueprints/Pawns/TheCovenWitchPlayerPawn"));
		if (!cls) {
			return;
		}
		UBlueprint* bp = Cast<UBlueprint>(cls);
		if (!bp) {
			return;
		}
		PC->UnPossess();
		TSubclassOf<class UObject> MyItemBlueprint;
		MyItemBlueprint = (UClass*)bp->GeneratedClass;

		FActorSpawnParameters SpawnParams;
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(MyItemBlueprint, { 0, 0, 0 }, { 0, 0, 0 }, SpawnParams);
		PC->Possess(NewCharacter);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ghost21"));
	}
}

void ATheCovenPlayerState::ClientInitialize(AController* InController)
{
	Super::Super::ClientInitialize(InController);

	UpdatePlayerPawn();
}

void ATheCovenPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdatePlayerPawn();
}

void ATheCovenPlayerState::OnRep_TeamColor()
{
	UpdatePlayerPawn();
}