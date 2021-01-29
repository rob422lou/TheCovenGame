// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TheCoven/TheCovenCharacterMovement.h"
#include "TheCoven/Player/TheCovenCharacter.h"

ATheCovenCharacter::ATheCovenCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTheCovenCharacterMovement>(ACharacter::CharacterMovementComponentName))
{

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