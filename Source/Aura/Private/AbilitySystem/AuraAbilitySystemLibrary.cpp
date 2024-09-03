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
#include "AuraAbilityTypes.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"

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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, const ECharacterClass CharacterClass, const float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (AuraGameModeBase == nullptr) return;

	const AActor* AvatarActor = ASC->GetAvatarActor();

	const FCharacterClassDefaultInfo ClassDefaultInfo = AuraGameModeBase->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

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

void UAuraAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, const ECharacterClass Class)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if (AuraGameModeBase == nullptr) return;

	//Common abilities
	for (const TSubclassOf<UGameplayAbility>& Ability :AuraGameModeBase->CharacterClassInfo->CommonAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, 1));
	}
	//Class abilities
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
	check(CombatInterface);
	for (const TSubclassOf<UGameplayAbility>& Ability: AuraGameModeBase->CharacterClassInfo->CharacterClassInformation[Class].ClassAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, CombatInterface->GetCharacterLevel()));
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

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, const float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
	}
	for (FOverlapResult& Overlap: Overlaps)
	{
		//If implements combat interface and is dead
		if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
		{
			OutOverlappingActors.AddUnique(Overlap.GetActor());
		}
	}
}

FVector UAuraAbilitySystemLibrary::GetCombatSocketLocation(const AAuraCharacterBase* Character, const FGameplayTag& Tag)
{
	const TMap<FGameplayTag, FName> TagToSocketMap = FAuraGameplayTags::Get().TagToSocketName;
	const FName& SocketName = TagToSocketMap[Tag];
	if (!SocketName.Compare(FName("WeaponSocket")))
		return Character->GetWeapon()->GetSocketLocation(SocketName);
	return Character->GetMesh()->GetSocketLocation(SocketName);
}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	//return false if both have Player tag or Enemy tag
	return !(FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player")) ||
		(FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"))));
}

TArray<AAuraPlayerController*> UAuraAbilitySystemLibrary::GetAllPlayerControllers(const AActor* WorldContextObject)
{
	return Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject))->GetAllPlayerControllers();
}
