// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "../Actors/TheCovenActor.h"
#include "TheCoven/TheCovenCharacterMovement.h"
#include "TheCoven/Player/TheCovenCharacter.h"

ATheCovenCharacter::ATheCovenCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTheCovenCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	GetMesh1P()->bOnlyOwnerSee = false;
	GetMesh1P()->bOwnerNoSee = false;
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = false;
	MaxUseDistance = 2500.0f;
}

void ATheCovenCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	if (bIsThirdPerson) {
		USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
		const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
		const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

		// Mesh rotating code expect uniform scale in LocalToWorld matrix

		const FRotator RotCameraPitch(0.0f, 0.0f, 0.0f);
		const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

		const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
		const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
		const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
		const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

		//GetMesh1P()->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
		GetMesh1P()->SetRelativeRotation(PitchedMesh.Rotator());
		return;
	}

	Super::OnCameraUpdate(CameraLocation, CameraRotation);
}

void ATheCovenCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Controller && Controller->IsLocalController())
	{
		ATheCovenActor* usable = GetTheCovenActorInView();

		// End Focus
		if (FocusedUsableActor != usable)
		{
			if (FocusedUsableActor)
			{
				FocusedUsableActor->EndFocusItem();
			}

			bHasNewFocus = true;
		}

		// Assign new Focus
		FocusedUsableActor = usable;

		// Start Focus.
		if (usable && (usable->GetIsGhostUsable() || usable->GetIsWitchUsable()))
		{
			if (bHasNewFocus)
			{
				usable->StartFocusItem();
				bHasNewFocus = false;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATheCovenCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &APerdixCharacter::OnStartMouseWheelUp);
	//PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &APerdixCharacter::OnStartMouseWheelDown);
	//PlayerInputComponent->BindAction("Noclip", IE_Pressed, this, &APerdixCharacter::Noclip);
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ATheCovenCharacter::OnStartUse);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &ATheCovenCharacter::OnStopUse);
	PlayerInputComponent->BindAction("SkillOne", IE_Pressed, this, &ATheCovenCharacter::OnSkillOneUse);
	PlayerInputComponent->BindAction("SkillTwo", IE_Pressed, this, &ATheCovenCharacter::OnSkillTwoUse);
	PlayerInputComponent->BindAction("SkillThree", IE_Pressed, this, &ATheCovenCharacter::OnSkillThreeUse);
	PlayerInputComponent->BindAction("SkillFour", IE_Pressed, this, &ATheCovenCharacter::OnSkillFourUse);
	//PlayerInputComponent->BindAction("QuickMenu", IE_Pressed, this, &APerdixCharacter::OnStartQuickMenu);
	//PlayerInputComponent->BindAction("QuickMenu", IE_Released, this, &APerdixCharacter::OnStopQuickMenu);

	/*
	const FName name = FName("Reload");
	const FKey oldKey = FKey("R");

	const UInputSettings* DefaultInputSettings = GetDefault<UInputSettings>();
	const FInputActionKeyMapping fInputActionKeyMapping(name, oldKey);

	// This is to remove binding that's set in shootergamecharacter.cpp
	// Obviously it makes more sense to just change it there, but I'm trying to keep 
	// all code in Perdix classes to make updating the engine easier.
	for (int i = 0; i < PlayerInputComponent->GetNumActionBindings(); i++) {
		if (PlayerInputComponent->GetActionBinding(i).GetActionName() == "Reload")
		{
			PlayerInputComponent->RemoveActionBinding(i);
			i = PlayerInputComponent->GetNumActionBindings();
		}
	}

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APerdixCharacter::OnStartReload);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &APerdixCharacter::OnStopReload);
	*/
}

/*
Performs raytrace to find closest looked-at UsableActor.
*/

ATheCovenActor* ATheCovenCharacter::GetTheCovenActorInView()
{
	FVector camLoc;
	FRotator camRot;

	if (Controller == NULL)
		return NULL;

	Controller->GetPlayerViewPoint(camLoc, camRot);

	const FVector start_trace = camLoc;
	const FVector direction = camRot.Vector();
	const FVector end_trace = start_trace + (direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, start_trace, end_trace, ECC_GameTraceChannel2, TraceParams);

	/*DrawDebugLine(
		GetWorld(),
		start_trace,
		end_trace,
		FColor(255, 0, 0),
		false, 1, 0,
		12.333);*/

	return Cast<ATheCovenActor>(Hit.GetActor());
}

///////////////////////////////////////////////////////////////////////////////
// Use

void ATheCovenCharacter::OnStartUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnStartUse();
	}

	ATheCovenActor* usable = GetTheCovenActorInView();
	if (usable && (usable->GetIsGhostUsable() || usable->GetIsWitchUsable()))
	{
		CurrentlyUsed = usable;
		usable->OnBeginUse(this);
	}
}

void ATheCovenCharacter::ServerOnStartUse_Implementation()
{
	OnStartUse();
}

bool ATheCovenCharacter::ServerOnStartUse_Validate()
{
	// No special server-side validation performed.
	return true;
}

void ATheCovenCharacter::OnStopUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnStopUse();
	}
	if (CurrentlyUsed && (CurrentlyUsed->GetIsGhostUsable() || CurrentlyUsed->GetIsWitchUsable()))
	{
		CurrentlyUsed->OnEndUse(this);
		CurrentlyUsed = nullptr;
	}
}

void ATheCovenCharacter::ServerOnStopUse_Implementation()
{
	OnStopUse();
}

bool ATheCovenCharacter::ServerOnStopUse_Validate()
{
	// No special server-side validation performed.
	return true;
}

///////////////////////////////////////////////////////////////////////
// Skills

void ATheCovenCharacter::OnSkillOneUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnSkillOneUse();
	}
}

void ATheCovenCharacter::ServerOnSkillOneUse_Implementation()
{
	OnSkillOneUse();
}

bool ATheCovenCharacter::ServerOnSkillOneUse_Validate()
{
	// No special server-side validation performed.
	return true;
}

void ATheCovenCharacter::OnSkillTwoUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnSkillTwoUse();
	}
}

void ATheCovenCharacter::ServerOnSkillTwoUse_Implementation()
{
	OnSkillTwoUse();
}

bool ATheCovenCharacter::ServerOnSkillTwoUse_Validate()
{
	// No special server-side validation performed.
	return true;
}

void ATheCovenCharacter::OnSkillThreeUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnSkillThreeUse();
	}
}

void ATheCovenCharacter::ServerOnSkillThreeUse_Implementation()
{
	OnSkillThreeUse();
}

bool ATheCovenCharacter::ServerOnSkillThreeUse_Validate()
{
	// No special server-side validation performed.
	return true;
}

void ATheCovenCharacter::OnSkillFourUse_Implementation()
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOnSkillFourUse();
	}
}

void ATheCovenCharacter::ServerOnSkillFourUse_Implementation()
{
	OnSkillFourUse();
}

bool ATheCovenCharacter::ServerOnSkillFourUse_Validate()
{
	// No special server-side validation performed.
	return true;
}
/*
void ATheCovenCharacter::SetHidden_Implementation()
{

	if (GetLocalRole() < ROLE_Authority) {
		ServerSetHidden();
	}

	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATheCovenCharacter::ServerSetHidden_Implementation()
{
	//SetHidden();
}

bool ATheCovenCharacter::ServerSetHidden_Validate()
{
	// No special server-side validation performed.
	return true;
}
*/
/*
void ATheCovenCharacter::SetHidden_Implementation(bool bIsHidden)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ClientSetHidden_Implementation"));

	if (bIsHidden) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetVisibility(false);
		
	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
	}
}
*/
/*
void ATheCovenCharacter::SetHidden(bool bIsHidden) {
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetHidden(bIsHidden);
	}

	if (GetLocalRole() == ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Server: SetHidden_Implementation"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Client: SetHidden_Implementation"));
	}

	if (bIsHidden) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetVisibility(false);

	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
	}

}

void ATheCovenCharacter::ServerSetHidden_Implementation(bool bIsHidden) {
	if (GetLocalRole() == ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Server: ServerSetHidden_Implementation"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client: ServerSetHidden_Implementation"));
	}

	SetHidden(bIsHidden);

	if (bIsHidden) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetVisibility(false);

	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
	}

}
*/
/*
void ATheCovenCharacter::SetHidden(bool bIsHidden)
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

	if (bIsHidden) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetVisibility(false);

	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
	}
}

bool ATheCovenCharacter::ServerSetHidden_Validate(bool bIsHidden)
{
	return true;
}

void ATheCovenCharacter::ServerSetHidden_Implementation(bool bIsHidden)
{
	SetHidden(bIsHidden);
}
*/

ATheCovenPlayerController* ATheCovenCharacter::GetTheCovenPlayerController()
{
	return Cast<ATheCovenPlayerController>(Controller);
}

void ATheCovenCharacter::SetHidden()
{
	if (GetLocalRole() == ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Server: ServerSetHidden_Implementation"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Client: ServerSetHidden_Implementation"));
	}

	if (bIsHidden) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetVisibility(false);

	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
	}
}

void ATheCovenCharacter::OnRep_SetHidden()
{
	SetHidden();
}

void ATheCovenCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ATheCovenCharacter, bIsHidden);
}

void ATheCovenCharacter::OnStartFire_Implementation()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerOnStartFire();
	}
}

void ATheCovenCharacter::ServerOnStartFire_Implementation()
{
	OnStartFire();
}

bool ATheCovenCharacter::ServerOnStartFire_Validate()
{
	return true;
}

void ATheCovenCharacter::OnStartTargeting_Implementation()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerOnStartTargeting();
	}
}

void ATheCovenCharacter::ServerOnStartTargeting_Implementation()
{
	OnStartTargeting();
}

bool ATheCovenCharacter::ServerOnStartTargeting_Validate()
{
	return true;
}

void ATheCovenCharacter::OnStopTargeting_Implementation()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerOnStopTargeting();
	}
}

void ATheCovenCharacter::ServerOnStopTargeting_Implementation()
{
	OnStopTargeting();
}

bool ATheCovenCharacter::ServerOnStopTargeting_Validate()
{
	return true;
}