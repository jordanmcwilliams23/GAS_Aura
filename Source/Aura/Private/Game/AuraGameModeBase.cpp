// Copyright Jordan McWilliams


#include "Game/AuraGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

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

UAbilityInfo* AAuraGameModeBase::GetAbilityInfo() const
{
	return AbilityInfo;
}
