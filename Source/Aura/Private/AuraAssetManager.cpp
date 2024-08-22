// Copyright Jordan McWilliams


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	check(GEngine->AssetManager);
	return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	//This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();

	FAuraGameplayTags::InitializeNativeTags();
}
