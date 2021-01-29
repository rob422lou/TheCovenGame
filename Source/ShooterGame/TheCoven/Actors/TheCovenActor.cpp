// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TheCoven/Actors/TheCovenActor.h"

// Sets default values
ATheCovenActor::ATheCovenActor()
{
	bIsGhostUsable = false;
	bIsWitchUsable = false;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATheCovenActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATheCovenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//////////////////////////////////////////////////////////////////////////
// Renderer Functions

void ATheCovenActor::SetOutlineColor(EStencilColor OutlineColor)
{
	//Cosmetics don't need to be drawn for dedicated servers
	if (GetNetMode() != ENetMode::NM_DedicatedServer) {
		TArray<UActorComponent*> ComponentArray = GetComponentsByClass(UStaticMeshComponent::StaticClass());

		//Turn off outline (may yield renderer processing benefits?)
		if (OutlineColor == EStencilColor::SC_None) {
			for (UActorComponent* Comp : ComponentArray)
			{
				Cast<UStaticMeshComponent>(Comp)->SetRenderCustomDepth(false);
				Cast<UStaticMeshComponent>(Comp)->SetCustomDepthStencilValue((int32)OutlineColor);
			}
		}
		else {
			for (UActorComponent* Comp : ComponentArray)
			{
				Cast<UStaticMeshComponent>(Comp)->SetCustomDepthStencilValue((int32)OutlineColor);
				Cast<UStaticMeshComponent>(Comp)->SetRenderCustomDepth(true);
			}
		}
	}
}