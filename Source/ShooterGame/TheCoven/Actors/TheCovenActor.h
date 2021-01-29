// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheCovenActor.generated.h"

//Stencil colors, defined in PPI_StencilColor
UENUM(BlueprintType)
enum class EStencilColor : uint8
{
	SC_None = 0		UMETA(DisplayName = "None"),
	SC_LightBlue = 252 	UMETA(DisplayName = "Green"),
	SC_Red = 253	UMETA(DisplayName = "Light Blue"),
	SC_White = 254	UMETA(DisplayName = "Red") //255 is the max value
};

UCLASS()
class SHOOTERGAME_API ATheCovenActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheCovenActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//////////////////////////////////////////////////////////////////////////
// Use & Focus Functions
public:
	UPROPERTY(Category = "The Coven|Use", EditAnywhere, BlueprintReadWrite)
	bool bIsGhostUsable;
	UPROPERTY(Category = "The Coven|Use", EditAnywhere, BlueprintReadWrite)
	bool bIsWitchUsable;

	bool GetIsGhostUsable() { return bIsGhostUsable; }
	bool GetIsWitchUsable() { return bIsWitchUsable; }

	// [Server + Client]  
	UFUNCTION(BlueprintNativeEvent, Category = "The Coven|Use")
	void OnBeginUse(ATheCovenCharacter* character);
	void OnBeginUse_Implementation(ATheCovenCharacter* character) {}

	// [Server + Client]  
	UFUNCTION(BlueprintNativeEvent, Category = "The Coven|Use")
	void OnEndUse(ATheCovenCharacter* character);
	void OnEndUse_Implementation(ATheCovenCharacter* character) {}

	UFUNCTION(BlueprintImplementableEvent, Category = "The Coven|Use")
	bool StartFocusItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "The Coven|Use")
	bool EndFocusItem();

//////////////////////////////////////////////////////////////////////////
// Renderer Functions
	UFUNCTION(BlueprintCallable, Category = "The Coven")
	void SetOutlineColor(EStencilColor OutlineColor);
};
