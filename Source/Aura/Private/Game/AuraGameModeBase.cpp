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

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	USaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	} else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

void AAuraGameModeBase::TravelToMap(const UMVVM_LoadSlot* LoadSlot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, Levels.FindChecked(LoadSlot->GetMapName()));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Levels.Add(DefaultMapName, DefaultMap);
}
