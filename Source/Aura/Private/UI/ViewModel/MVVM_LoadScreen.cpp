// Copyright Jordan McWilliams


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(const int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

ENewSlotResponse UMVVM_LoadScreen::NewSlotButtonPressed(const int32 Slot, const FString& EnteredName)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (EnteredName.IsEmpty()) return EmptyName;
	if (!IsValid(AuraGameMode))
	{
		GEngine->AddOnScreenDebugMessage(5, 15.f, FColor::Magenta, FString("Error: switch to single player"));
		return Multiplayer;
	}

	UMVVM_LoadSlot* LoadSlot = LoadSlots[Slot];
	LoadSlot->SetMapName(AuraGameMode->DefaultMapName);
	LoadSlot->SetPlayerName(EnteredName);
	LoadSlot->SlotStatus = Taken;
	LoadSlot->SetPlayerLevel(1);
	LoadSlot->MapAssetName = AuraGameMode->DefaultMap.ToSoftObjectPath().GetAssetName();

	AuraGameMode->SaveSlotData(LoadSlot, Slot);
	LoadSlot->InitializeSlot();

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->LoadSlotName = LoadSlot->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex = LoadSlot->SlotIndex;
	AuraGameInstance->PlayerStartTag = LoadSlot->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
	return Success;
}

void UMVVM_LoadScreen::NewGameButtonPressed(const int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(const int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*>& Pair : LoadSlots)
	{
		Pair.Value->EnableSelectSlotButton.Broadcast(Pair.Key != Slot);
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::LoadData()
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode)) return;
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		const ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);

		const FString PlayerName = SaveObject->PlayerName;
		const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;

		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->InitializeSlot();
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
		
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;
	AAuraGameModeBase::DeleteSlot(SelectedSlot->GetLoadSlotName(), SelectedSlot->SlotIndex);
	SelectedSlot->SlotStatus = Vacant;
	SelectedSlot->InitializeSlot();
	SelectedSlot->EnableSelectSlotButton.Broadcast(true);
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	if (IsValid(SelectedSlot))
		AuraGameModeBase->TravelToMap(SelectedSlot);
}

void UMVVM_LoadScreen::SetNumLoadSlots(const int32 InNumLoadSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumLoadSlots);
}

void UMVVM_LoadScreen::InitializeAllSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex = 0;

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_1->SlotIndex = 1;

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlots.Add(2, LoadSlot_2);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;

	SetNumLoadSlots(LoadSlots.Num());
}
