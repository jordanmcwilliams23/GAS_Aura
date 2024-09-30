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
#include "AuraGameplayTags.h"
#include "AuraNamedArguments.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContext, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			OutAuraHUD = AuraHUD;
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AttributeSet = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			OutWCParams.AttributeSet = AttributeSet;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			OutWCParams.AbilitySystemComponent = ASC;
			return true;
		}
	}
	return false; 
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	//Check if widget controller params were made successfully
	if (MakeWidgetControllerParams(WorldContext, WCParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr; 
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	//Check if widget controller params were made successfully
	if (MakeWidgetControllerParams(WorldContext, WCParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr; 
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	//Check if widget controller params were made successfully
	if (MakeWidgetControllerParams(WorldContext, WCParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr; 
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, const ECharacterClass CharacterClass, const float Level, UAbilitySystemComponent* ASC)
{
	const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext));
	if (GameModeInterface == nullptr) return;

	const AActor* AvatarActor = ASC->GetAvatarActor();
	UCharacterClassInfo* CharacterClassInfo = GameModeInterface->GetCharacterClassInfo();
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimarySpec = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpec.Data.Get());

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpec = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpec = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, const ECharacterClass Class)
{
	const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext));
	if (GameModeInterface == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = GameModeInterface->GetCharacterClassInfo();
	//Common abilities
	for (const TSubclassOf<UGameplayAbility>& Ability :CharacterClassInfo->CommonAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, 1));
	}
	//Class abilities
	int32 Level = 1.f;
	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
	{
		Level = ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor());
	}
	for (const TSubclassOf<UGameplayAbility>& Ability:CharacterClassInfo->CharacterClassInformation[Class].ClassAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, Level));
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContext)
{
	if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext)))
	{
		return GameModeInterface->GetCharacterClassInfo();
	}
	return nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContext)
{
	if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext)))
	{
		return GameModeInterface->GetAbilityInfo();
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

float UAuraAbilitySystemLibrary::GetXPAmount(const UObject* WorldContext, const ECharacterClass Class, const int Level)
{
	const IGameModeInterface* GameModeInterface = CastChecked<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext));
	return GameModeInterface->GetCharacterClassInfo()->CharacterClassInformation[Class].XPAmount.GetValueAtLevel(Level);
}

TArray<FGameplayTag> UAuraAbilitySystemLibrary::CallerMagnitudeTags(const TSubclassOf<UGameplayEffect> GameplayEffect)
{
	const UGameplayEffect* GE = GameplayEffect.GetDefaultObject();
	const TArray<FGameplayModifierInfo> ModifierInfo = GE->Modifiers;
	TArray<FGameplayTag> CallerTags;

	for (const FGameplayModifierInfo& Info: ModifierInfo)
	{
		if (Info.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			CallerTags.Add(Info.ModifierMagnitude.GetSetByCallerFloat().DataTag);
		}
	}
	return CallerTags;
}

FString UAuraAbilitySystemLibrary::GetAbilityDescription(const UObject* WorldContextObject,
	const FGameplayTag& AbilityTag)
{
	FAuraAbilityInfo Info = GetAbilityInfo(WorldContextObject)->FindAbilityInfoForTag(AbilityTag);
	const int32 Level = Info.Ability->GetDefaultObject<UGameplayAbility>()->GetAbilityLevel();
	UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Info.Ability.GetDefaultObject());
	FormatAbilityDescriptionAtLevel(Ability, Info.Description, Info.DamageType,Level);
	return FString::Printf(
		TEXT(
		"<Title>%s </>\n"
		"<Title>Level %d</>\n"
		"%s\n"
		"\n"
		"<Default>Mana - </><Mana>%d</>\n"
		"<Default>Cooldown - </><Cooldown>%3.1fs</>"
		),
		*Info.Name.ToString(),
		Level,
		*Info.Description.ToString(),
		FMath::RoundToInt(-Ability->GetManaCost(Level)),
		Ability->GetCooldown(Level)
	);
}

FString UAuraAbilitySystemLibrary::GetAbilityNextLevelDescription(const UObject* WorldContextObject,
	const FGameplayTag& AbilityTag)
{
	FAuraAbilityInfo Info = GetAbilityInfo(WorldContextObject)->FindAbilityInfoForTag(AbilityTag);
	const int32 Level = Info.Ability->GetDefaultObject<UGameplayAbility>()->GetAbilityLevel();
	UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Info.Ability.GetDefaultObject());
	FormatAbilityDescriptionAtLevel(Ability, Info.NextLevelDescription, Info.DamageType,Level);
	return FString::Printf(
		TEXT(
		"<Title>%s</>\n"
		"<Title>Level </><Old>%d</><Level> > %d</>\n"
		"%s\n"
		"\n"
		"<Default>Mana - </><Old>%d</><Default> > </><Mana>%d</>\n"
		"<Default>Cooldown - </><Old>%3.1fs</><Default> > </><Cooldown>%3.1fs</>"
		),
		*Info.Name.ToString(),
		Level,
		Level+1,
		*Info.NextLevelDescription.ToString(),
		FMath::RoundToInt(-Ability->GetManaCost(Level)),
		FMath::RoundToInt(-Ability->GetManaCost(Level+1)),
		Ability->GetCooldown(Level),
		Ability->GetCooldown(Level+1)
	);
}

void UAuraAbilitySystemLibrary::FormatAbilityDescriptionAtLevel(UGameplayAbility* Ability, FText& OutDescription, const FGameplayTag& DamageType, const int32 Level)
{
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	if (UAuraDamageGameplayAbility* DamageAbility = Cast<UAuraDamageGameplayAbility>(Ability))
	{
		const FAuraNamedArguments Args;
		OutDescription = FText::FormatNamed(
		OutDescription,
		Args.Damage0,
		DamageAbility->GetRoundedDamageAtLevel(Level),
		Args.Damage1,
		DamageAbility->GetRoundedDamageAtLevel(Level + 1)
		);
	}
}