// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContext)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			const FWidgetControllerParams Params(PC, PS, ASC, AttributeSet);
			return AuraHUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr; 
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContext)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(Params);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (AuraGameModeBase == nullptr) return;

	AActor* AvatarActor = ASC->GetAvatarActor();
	
	FCharacterClassDefaultInfo ClassDefaultInfo = AuraGameModeBase->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimarySpec = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpec.Data.Get());

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpec = ASC->MakeOutgoingSpec(AuraGameModeBase->CharacterClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpec = ASC->MakeOutgoingSpec(AuraGameModeBase->CharacterClassInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (AuraGameModeBase == nullptr) return;
	
	for (const TSubclassOf<UGameplayAbility> Ability :AuraGameModeBase->CharacterClassInfo->CommonAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, 1));
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContext)
{
	if (const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext)))
	{
		return AuraGameMode->CharacterClassInfo;
	}
	return nullptr;
}
