// Copyright Jordan McWilliams


#include "Game/AuraGameModeBase.h"

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

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
