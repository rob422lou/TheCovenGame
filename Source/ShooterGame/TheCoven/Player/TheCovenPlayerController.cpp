// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TheCoven/Player/TheCovenCharacter.h"
#include "TheCoven/Player/TheCovenPlayerController.h"

void ATheCovenPlayerController::SetHidden(bool bIsHidden)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetHidden(bIsHidden);
	}

	if (GetLocalRole() == ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Server: ServerSetHidden_Implementation"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client: ServerSetHidden_Implementation"));
	}

	ATheCovenCharacter* TheCovenCharacter = (ATheCovenCharacter*)GetPawn();
	if (!TheCovenCharacter)
	{
		return;
	}
	if (bIsHidden) {
		TheCovenCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TheCovenCharacter->GetMesh()->SetVisibility(false);

	}
	else
	{
		TheCovenCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		TheCovenCharacter->GetMesh()->SetVisibility(true);
	}
}

bool ATheCovenPlayerController::ServerSetHidden_Validate(bool bIsHidden)
{
	return true;
}

void ATheCovenPlayerController::ServerSetHidden_Implementation(bool bIsHidden)
{
	SetHidden(bIsHidden);
}