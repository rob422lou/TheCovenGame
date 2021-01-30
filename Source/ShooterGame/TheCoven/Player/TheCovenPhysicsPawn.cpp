// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TheCoven/TheCovenCharacterMovement.h"
#include "TheCoven/Player/TheCovenPhysicsPawn.h"

// Sets default values
ATheCovenPhysicsPawn::ATheCovenPhysicsPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTheCovenCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	bReplicates = true;

	PlayerStaticMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PawnMesh"));

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATheCovenPhysicsPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerStaticMesh->SetSimulatePhysics(true);
	PlayerStaticMesh->SetIsReplicated(true);
}

// Called every frame
void ATheCovenPhysicsPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATheCovenPhysicsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ATheCovenPhysicsPawn::OnStartUse);
}

///////////////////////////////////////////////////////////////////////////////
// Use

void ATheCovenPhysicsPawn::OnStartUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnStartUse();
	}
}

void ATheCovenPhysicsPawn::ServerOnStartUse_Implementation()
{
	OnStartUse();
}

bool ATheCovenPhysicsPawn::ServerOnStartUse_Validate()
{
	// No special server-side validation performed.
	return true;
}
