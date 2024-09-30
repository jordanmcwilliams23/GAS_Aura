// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interaction/GameModeInterface.h"
#include "AuraGameModeBase.generated.h"

class ULevelUpInfo;
class UCharacterClassInfo;
class AAuraPlayerController;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase, public IGameModeInterface
{
	GENERATED_BODY()
public:
	TArray<AAuraPlayerController*> GetAllPlayerControllers() {return PlayerControllers;}
	
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
	virtual UAbilityInfo* GetAbilityInfo() const override;
	/* End Game Mode Interface */

	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
private:
	TArray<AAuraPlayerController*> PlayerControllers;

	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	int NumGamePausedSources = 0;
};
