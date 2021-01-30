// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TheCovenPhysicsPawn.generated.h"

UCLASS()
class SHOOTERGAME_API ATheCovenPhysicsPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this pawn's properties
	ATheCovenPhysicsPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* PlayerStaticMesh;

public:
	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintPure, Category = TheCoven)
	FORCEINLINE UStaticMeshComponent* GetPlayerStaticMesh() const { return PlayerStaticMesh; }

//////////////////////////////////////////////////////////////////////////
// Input handlers
public:
	/** player pressed use action */
	UFUNCTION(BlueprintNativeEvent, Category = PlayerAbility)
	void OnStartUse();

	UFUNCTION(WithValidation, Server, Reliable)
	void ServerOnStartUse();

};
