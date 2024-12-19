// Copyright Jordan McWilliams


#include "Game/AuraGameInstance.h"

#include "Player/AuraPlayerController.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAuraGameInstance::LoadedWorld);
}

void UAuraGameInstance::LoadedWorld(UWorld* World)
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetPrimaryPlayerController());
	AuraPlayerController->CachedDestination = InitialMoveToLocation;
	AuraPlayerController->BlockInputAndMoveToCachedDestination();
	BlueprintLoadedWorld(World);
}
