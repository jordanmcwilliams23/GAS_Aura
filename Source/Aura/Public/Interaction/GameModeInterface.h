// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeInterface.generated.h"

class AAuraPlayerController;
class UAbilityInfo;
class UCharacterClassInfo;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IGameModeInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	virtual void PauseGame(const UObject* WorldContextObject) = 0;
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	virtual void UnpauseGame(const UObject* WorldContextObject) = 0;

	UFUNCTION(BlueprintCallable)
	virtual UCharacterClassInfo* GetCharacterClassInfo() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual TArray<AAuraPlayerController*> GetAuraPlayerControllers() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void LoadWorldState(UWorld* World) const = 0;
};
