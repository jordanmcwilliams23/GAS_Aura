// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interaction/GameModeInterface.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class ULevelUpInfo;
class UCharacterClassInfo;
class AAuraPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerDiedSignature, const ACharacter*, DeadCharacter);
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase, public IGameModeInterface
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/* Game Mode Interface */
	UFUNCTION()
	virtual void PauseGame(const UObject* WorldContextObject) override;
	UFUNCTION()
	virtual void UnpauseGame(const UObject* WorldContextObject) override;
	UFUNCTION()
	virtual UCharacterClassInfo* GetCharacterClassInfo() const override;
	UFUNCTION()
	virtual TArray<AAuraPlayerController*> GetAuraPlayerControllers() const override;
	UFUNCTION()
	virtual void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const override;
	UFUNCTION()
	virtual void LoadWorldState(UWorld* World) const override;
	/* End Game Mode Interface */

	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category="Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot,int32 SlotIndex) const;
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Levels;

	FString GetMapNameFromMapAssetName(const FString& InMapAssetName) const;

	void TravelToMap(const UMVVM_LoadSlot* LoadSlot);
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	ULoadScreenSaveGame* RetrieveInGameSaveData() const;
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const;

	void RestartLevel(const ACharacter* PlayerCharacter) const;

	UPROPERTY(BlueprintAssignable)
	FPlayerDiedSignature OnPlayerDied;
protected:
	virtual void BeginPlay() override;
private:
	TArray<AAuraPlayerController*> PlayerControllers;

	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	int NumGamePausedSources = 0;
};
