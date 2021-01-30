// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Misc/NetworkVersion.h"
#include "Net/UnrealNetwork.h"
#include "../Player/TheCovenCharacter.h"
#include "../Player/TheCovenPlayerController.h"
#include "TheCoven/UI/TheCovenHUD.h"


// For localization. Used with LOCTEXT();
#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

void ATheCovenHUD::DrawHUD()
{
	//Super::Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}
	ScaleUI = Canvas->ClipY / 1080.0f;

	// Empty the info item array
	InfoItems.Empty();
	float TextScale = 1.0f;
	ScaleUI = FMath::Max(ScaleUI, MinHudScale);

	ATheCovenCharacter* MyPawn = Cast<ATheCovenCharacter>(GetOwningPawn());

	float MessageOffset = (Canvas->ClipY / 4.0) * ScaleUI;
	if (MatchState == EShooterMatchState::Playing)
	{
		ATheCovenPlayerController* MyPC = Cast<ATheCovenPlayerController>(PlayerOwner);
		if (MyPC)
		{
			// PERDIX
			//DrawKills();
		}

		if (MyPawn && MyPawn->IsAlive())
		{
			DrawHealth();
			//DrawWeaponHUD();

			DrawTimer();
		}
		else
		{
			// respawn
			FString Text = LOCTEXT("WaitingForRespawn", "WAITING FOR RESPAWN").ToString();
			FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark);
			TextItem.EnableShadow(FLinearColor::Black);
			TextItem.Text = FText::FromString(Text);
			TextItem.Scale = FVector2D(TextScale * ScaleUI, TextScale * ScaleUI);
			TextItem.FontRenderInfo = ShadowedFont;
			TextItem.SetColor(HUDLight);
			AddMatchInfoString(TextItem);
		}

		//DrawDeathMessages();
		DrawCrosshair();
		DrawHitIndicator();

		// Draw any recent killed player - cache the used Y coord for later when we draw the large onscreen messages.
		MessageOffset = DrawRecentlyKilledPlayer();

		// No ammo message if required
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - NoAmmoNotifyTime >= 0 && CurrentTime - NoAmmoNotifyTime <= NoAmmoFadeOutTime)
		{
			FString Text = FString();

			const float Alpha = FMath::Min(1.0f, 1 - (CurrentTime - NoAmmoNotifyTime) / NoAmmoFadeOutTime);
			Text = LOCTEXT("NoAmmo", "NO AMMO").ToString();

			FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark);
			TextItem.EnableShadow(FLinearColor::Black);
			TextItem.Text = FText::FromString(Text);
			TextItem.Scale = FVector2D(TextScale * ScaleUI, TextScale * ScaleUI);
			TextItem.FontRenderInfo = ShadowedFont;
			TextItem.SetColor(FLinearColor(0.75f, 0.125f, 0.125f, Alpha));
			AddMatchInfoString(TextItem);
		}

		UNetDriver* NetDriver = GetNetDriver();

		if (GetNetMode() != NM_Standalone)
		{
			FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");

			IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
			if (OnlineSubsystem)
			{
				IOnlineSessionPtr SessionSubsystem = OnlineSubsystem->GetSessionInterface();
				if (SessionSubsystem.IsValid())
				{
					FNamedOnlineSession* Session = SessionSubsystem->GetNamedSession(GameSessionName);
					if (Session)
					{
						NetModeDesc += TEXT("\nSession: ");
						if (Session->SessionInfo) {
							NetModeDesc += Session->SessionInfo->GetSessionId().ToString();
						}
						else {
							NetModeDesc += " Steam";
						}
					}
				}

			}

			NetDriver->bCollectNetStats = true;
			//FString InStr = FString::SanitizeFloat((NetDriver->InBytesPerSecond / 10) / 100.f, 2);
			//FString OutStr = FString::SanitizeFloat((NetDriver->OutBytesPerSecond / 10) / 100.f, 2);

			FString InStr = FString::FromInt((NetDriver->InBytesPerSecond * 8) / 1000);
			FString OutStr = FString::FromInt((NetDriver->OutBytesPerSecond * 8) / 1000);

			if (PlayerOwner && PlayerOwner->PlayerState && GetNetMode() != ENetMode::NM_ListenServer) {
				NetModeDesc += FString::Printf(TEXT("\nVersion: %i, %s, %s\nFPS: %i\nPing: %i\nIn:	%s Kb/s\nOut:	%s Kb/s"), FNetworkVersion::GetNetworkCompatibleChangelist(), UTF8_TO_TCHAR(__DATE__), UTF8_TO_TCHAR(__TIME__), FMath::FloorToInt(1.f / GetWorld()->DeltaTimeSeconds), FMath::FloorToInt(PlayerOwner->PlayerState->ExactPing), *InStr, *OutStr);
			}
			else
			{
				NetModeDesc += FString::Printf(TEXT("\nVersion: %i, %s, %s\nFPS: %i\nPing: ---\nIn:	%s Kb/s\nOut:	%s Kb/s"), FNetworkVersion::GetNetworkCompatibleChangelist(), UTF8_TO_TCHAR(__DATE__), UTF8_TO_TCHAR(__TIME__), FMath::FloorToInt(1.f / GetWorld()->DeltaTimeSeconds), *InStr, *OutStr);
			}

			DrawDebugInfoString(NetModeDesc, Canvas->OrgX + (Offset * ScaleUI), Canvas->OrgY + (Offset * ScaleUI), true, true, HUDLight);
		}
	}

	// Render the info messages such as wating to respawn - these will be drawn below any 'killed player' message.
	ShowInfoItems(MessageOffset, 1.0f);
}

void ATheCovenHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor)
{
#if !UE_BUILD_SHIPPING
	float SizeX, SizeY; //Populated by StrLen
	Canvas->StrLen(NormalFont, Text, SizeX, SizeY); //SizeX is going to be too long because StrLen sums up all the text, including new lines.

	const float UsePosX = bAlignLeft ? PosX : PosX - SizeX;
	const float UsePosY = bAlignTop ? PosY : PosY - SizeY;

	const float BoxPadding = 5.0f;

	FColor DrawColor(HUDDark.R, HUDDark.G, HUDDark.B, HUDDark.A * 0.2f);
	const float X = UsePosX - (BoxPadding * ScaleUI);
	const float Y = UsePosY - (BoxPadding * ScaleUI);
	// hack in the *2.f scaling for Y since UCanvas::StrLen doesn't take into account newlines
	const float SCALE_X = 1.0; // Scales semitransparent background
	const float SCALE_Y = 5.8f; // Scales semitransparent background

	FCanvasTileItem TileItem(FVector2D(X, Y), FVector2D(((SizeX / 2) + BoxPadding * SCALE_X) * ScaleUI, (SizeY * SCALE_Y + BoxPadding * SCALE_Y) * ScaleUI), DrawColor);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	FCanvasTextItem TextItem(FVector2D(UsePosX, UsePosY), FText::FromString(Text), NormalFont, TextColor);
	TextItem.EnableShadow(FLinearColor::Black);
	TextItem.FontRenderInfo = ShadowedFont;
	TextItem.Scale = FVector2D(ScaleUI, ScaleUI);
	Canvas->DrawItem(TextItem);
#endif
}

void ATheCovenHUD::DrawHealth()
{
	ATheCovenCharacter* MyPawn = Cast<ATheCovenCharacter>(GetOwningPawn());
	//Canvas->SetDrawColor(FColor::White);
	const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL * ScaleUI) / 2;
	const float HealthPosY = Canvas->ClipY - (Offset + HealthBarBg.VL) * ScaleUI;
	//Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);
	const float HealthAmount = FMath::Min(1.0f, MyPawn->Health / MyPawn->GetMaxHealth());
	/*
	FCanvasTileItem TileItem(FVector2D(HealthPosX, HealthPosY), HealthBar.Texture->Resource,
		FVector2D(HealthBar.UL * HealthAmount * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);
	MakeUV(HealthBar, TileItem.UV0, TileItem.UV1, HealthBar.U, HealthBar.V, HealthBar.UL * HealthAmount, HealthBar.VL);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	Canvas->DrawIcon(HealthIcon, HealthPosX + Offset * ScaleUI, HealthPosY + (HealthBar.VL - HealthIcon.VL) / 2.0f * ScaleUI, ScaleUI);
	*/
	DrawText("Health: " + FString::FromInt(MyPawn->Health), FLinearColor::White, HealthPosX + Offset * ScaleUI, HealthPosY + (HealthBar.VL - HealthIcon.VL) / 2.0f * ScaleUI, BigFont, ScaleUI, false);
}

void ATheCovenHUD::DrawTimer()
{
	ATheCovenCharacter* MyPawn = Cast<ATheCovenCharacter>(GetOwningPawn());
	//Canvas->SetDrawColor(FColor::White);
	const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL * ScaleUI) / 2;
	const float HealthPosY = Canvas->ClipY - (Offset + HealthBarBg.VL) * ScaleUI;
	//Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);
	//const float HealthAmount = FMath::Min(1.0f, MyPawn->Health / MyPawn->GetMaxHealth());
	/*
	FCanvasTileItem TileItem(FVector2D(HealthPosX, HealthPosY), HealthBar.Texture->Resource,
		FVector2D(HealthBar.UL * HealthAmount * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);
	MakeUV(HealthBar, TileItem.UV0, TileItem.UV1, HealthBar.U, HealthBar.V, HealthBar.UL * HealthAmount, HealthBar.VL);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	Canvas->DrawIcon(HealthIcon, HealthPosX + Offset * ScaleUI, HealthPosY + (HealthBar.VL - HealthIcon.VL) / 2.0f * ScaleUI, ScaleUI);
	*/
	AShooterGameState* const MyGameState = GetWorld()->GetGameState<AShooterGameState>();
	if (MyGameState && MyGameState->RemainingTime > 0)
	{
		DrawText("Timer: " + FString::FromInt(MyGameState->RemainingTime), FLinearColor::White, 5, HealthPosY + (HealthBar.VL - HealthIcon.VL) / 2.0f * ScaleUI, BigFont, ScaleUI, false);
	}
}

#undef LOCTEXT_NAMESPACE