// Copyright Jordan McWilliams


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(NewPlayer))
		PlayerControllers.Add(AuraPlayerController);
}

void AAuraGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(Exiting))
		PlayerControllers.Remove(AuraPlayerController);
}

void AAuraGameModeBase::PauseGame(const UObject* WorldContextObject)
{
	NumGamePausedSources++;
	UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, 0.f);
}

void AAuraGameModeBase::UnpauseGame(const UObject* WorldContextObject)
{
	if (--NumGamePausedSources <= 0)
	{
		UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, 1.f);
	}
}

UCharacterClassInfo* AAuraGameModeBase::GetCharacterClassInfo() const
{
	return CharacterClassInfo;
}

TArray<AAuraPlayerController*> AAuraGameModeBase::GetAuraPlayerControllers() const
{
	return PlayerControllers;
}

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	}
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}
