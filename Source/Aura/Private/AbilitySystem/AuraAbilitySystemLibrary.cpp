// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
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
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/GameStateInterface.h"
#include "Player/AuraPlayerController.h"
#include "GameFramework/GameStateBase.h"

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
		return AuraHUD->GetOverlayWidgetController(WCParams);
	return nullptr; 
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	//Check if widget controller params were made successfully
	if (MakeWidgetControllerParams(WorldContext, WCParams, AuraHUD))
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	return nullptr; 
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellWidgetController(const UObject* WorldContext)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	//Check if widget controller params were made successfully
	if (MakeWidgetControllerParams(WorldContext, WCParams, AuraHUD))
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromDisc(const UObject* WorldContext,
	UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContext);
	//Return if on client
	if (CharacterClassInfo == nullptr) return;

	
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller, 1, Context);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Tags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Tags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Tags.Attributes_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Tags.Attributes_Primary_Vigor, SaveGame->Vigor);

	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);

	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SecondarySpec = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes_Infinite, 1.f, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());

	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle VitalSpec = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, 1.f, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, const ECharacterClass Class)
{
	const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext));
	if (GameModeInterface == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = GameModeInterface->GetCharacterClassInfo();
	//Common abilities
	for (const TSubclassOf<UGameplayAbility>& Ability :CharacterClassInfo->CommonAbilities)
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, 1));
	
	//Class abilities
	int32 Level = 1.f;
	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		Level = ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor());
	
	for (const TSubclassOf<UGameplayAbility>& Ability:CharacterClassInfo->CharacterClassInformation[Class].ClassAbilities)
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, Level));
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContext)
{
	if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContext)))
		return GameModeInterface->GetCharacterClassInfo();
	return nullptr;
}

ULootTiers* UAuraAbilitySystemLibrary::GetLootTiers(const UObject* WorldContext)
{
	if (const AAuraGameModeBase* GameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext)))
		return GameModeBase->LootTiers;
	return nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContext)
{
	if (const IGameStateInterface* GameStateInterface = Cast<IGameStateInterface>(UGameplayStatics::GetGameState(WorldContext)))
		return GameStateInterface->GetAbilityInfo();
	return nullptr;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->IsBlockedHit();
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->IsCriticalHit();
	return false;
}

bool UAuraAbilitySystemLibrary::DidApplyDebuff(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->AppliedDebuff();
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetDebuffDamage();
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetDebuffDuration();
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetDebuffFrequency();
	return 0.f;
}

bool UAuraAbilitySystemLibrary::GetShouldHitReact(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetShouldHitReact();
	return true;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		if (EffectContext->GetDamageType().IsValid())
			return *EffectContext->GetDamageType();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetDeathImpulse();
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetKnockbackForce();
	return FVector::ZeroVector;
}

float UAuraAbilitySystemLibrary::GetKnockbackChance(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetKnockbackChance();
	return 0.f;
}

bool UAuraAbilitySystemLibrary::GetIsRadialDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetIsRadialDamage();
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetRadialDamageInnerRadius();
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetRadialDamageOuterRadius();
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
		return EffectContext->GetRadialDamageOrigin();
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetBlockedHit(bInIsBlockedHit);
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetCriticalHit(bInIsCriticalHit);
}

void UAuraAbilitySystemLibrary::SetAppliedDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	const bool bInAppliedDebuff)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetAppliedDebuff(bInAppliedDebuff);
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle,
	const float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetDebuffDamage(InDebuffDamage);
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
	const float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetDebuffFrequency(InDebuffFrequency);
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,
	const float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetDebuffDuration(InDebuffDuration);
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetDamageType(MakeShared<FGameplayTag>(InDamageType));
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetDeathImpulse(InDeathImpulse);
}

void UAuraAbilitySystemLibrary::SetShouldHitReact(FGameplayEffectContextHandle& EffectContextHandle,
	const bool InShouldHitReact)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetShouldHitReact(InShouldHitReact);
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector InKnockbackForce)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetKnockbackForce(InKnockbackForce);
}

void UAuraAbilitySystemLibrary::SetKnockbackChance(FGameplayEffectContextHandle& EffectContextHandle,
	const float InKnockbackChance)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetKnockbackChance(InKnockbackChance);
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	const bool InIsRadialDamage)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetIsRadialDamage(InIsRadialDamage);
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageInnerRadius)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageOuterRadius)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector InRadialDamageOrigin)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
		EffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
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

void UAuraAbilitySystemLibrary::GetClosestTargets(const int32 MaxTargets, const TArray<AActor*>& Actors,
                                                  TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (MaxTargets >= Actors.Num())
	{
		OutClosestTargets = Actors;
		return;
	}
	if (Actors.IsEmpty()) return;

	TArray<AActor*> ActorsCopy = Actors;
	ActorsCopy.Sort(
		[Origin](const AActor& A, const AActor& B)
	{
		const float DistanceA = FVector::Distance(A.GetActorLocation(), Origin);
		const float DistanceB = FVector::Distance(B.GetActorLocation(), Origin);
			return DistanceA < DistanceB;
	});
	for (int i = 0; i < MaxTargets; i++)
		OutClosestTargets.Add(ActorsCopy[i]);
}

FVector UAuraAbilitySystemLibrary::GetCombatSocketLocation(const AAuraCharacterBase* Character, const FGameplayTag& Tag)
{
	const TMap<FGameplayTag, FName> TagToSocketMap = FAuraGameplayTags::Get().TagToSocketName;
	const FName& SocketName = TagToSocketMap[Tag];
	if (!SocketName.Compare(FName("WeaponSocket")))
	{
		if (Character->Implements<UCombatInterface>())
			return ICombatInterface::Execute_GetWeapon(Character)->GetSocketLocation(SocketName);
	}
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
	return Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(WorldContextObject))->GetAuraPlayerControllers();
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
	const FGameplayTag& AbilityTag, const int32 Level)
{
	FAuraAbilityInfo Info = GetAbilityInfo(WorldContextObject)->FindAbilityInfoForTag(AbilityTag);
	UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Info.Ability.GetDefaultObject());
	const bool bIsPassiveAbility = AbilityTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Passive);
	FormatAbilityDescriptionAtLevel(Ability, Info.Description, Info.DamageType,Level);
	FString AbilityDescription = FString::Printf(TEXT(
		"<Title>%s</>\n"
		"<Level>Level %d</>\n"
		"%s\n"
		),
		*Info.Name.ToString(),
		Level,
		*Info.Description.ToString()
	);
	if (!bIsPassiveAbility)
	{
		 AbilityDescription.Appendf(TEXT(
			"\n<Default>Mana - </><Mana>%d</>\n"
			"<Default>Cooldown - </><Cooldown>%3.1fs</>"),
			FMath::RoundToInt(-Ability->GetManaCost(Level)),
			Ability->GetCooldown(Level)
			); 
	}
	return AbilityDescription;
}

FString UAuraAbilitySystemLibrary::GetAbilityNextLevelDescription(const UObject* WorldContextObject,
	const FGameplayTag& AbilityTag, const int32 Level)
{
	FAuraAbilityInfo Info = GetAbilityInfo(WorldContextObject)->FindAbilityInfoForTag(AbilityTag);
	UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Info.Ability.GetDefaultObject());
	const bool bIsPassiveAbility = AbilityTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Passive);
	FormatAbilityDescriptionAtLevel(Ability, Info.NextLevelDescription, Info.DamageType,Level);
	FString AbilityDescription = FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n"
		"<Level>Level </><Old>%d</><Level> > %d</>\n"
		"%s\n"),
		Level,
		Level+1,
		*Info.NextLevelDescription.ToString()
		);
	if (!bIsPassiveAbility)
	{
		AbilityDescription.Appendf(TEXT(
			"\n"
			"<Default>Mana - </><Old>%d</><Default> > </><Mana>%d</>\n"
			"<Default>Cooldown - </><Old>%3.1fs</><Default> > </><Cooldown>%3.1fs</>"),
			FMath::RoundToInt(-Ability->GetManaCost(Level)),
			FMath::RoundToInt(-Ability->GetManaCost(Level+1)),
			Ability->GetCooldown(Level),
			Ability->GetCooldown(Level+1)
			);
	}
	return AbilityDescription;
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	if (!Params.TargetASC || !Params.SourceASC) return FGameplayEffectContextHandle();
	
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle Context = Params.SourceASC->MakeEffectContext();
	Context.AddSourceObject(Params.SourceASC->GetAvatarActor());
	const FGameplayEffectSpecHandle Spec = Params.SourceASC->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbilityLevel, Context);

	Spec.Data->SetSetByCallerMagnitude(Params.DamageType, Params.BaseDamage);
	Spec.Data->SetSetByCallerMagnitude(Tags.Debuff_Damage, Params.DebuffDamage);
	Spec.Data->SetSetByCallerMagnitude(Tags.Debuff_Chance, Params.DebuffChance);
	Spec.Data->SetSetByCallerMagnitude(Tags.Debuff_Frequency, Params.DebuffFrequency);
	Spec.Data->SetSetByCallerMagnitude(Tags.Debuff_Duration, Params.DebuffDuration);
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(Context.Get()))
	{
		EffectContext->SetDamageType(MakeShared<FGameplayTag>(Params.DamageType));
		EffectContext->SetDeathImpulse(Params.DeathImpulse);
		EffectContext->SetKnockbackForce(Params.KnockbackForce);
		EffectContext->SetKnockbackChance(Params.KnockbackChance);
		if (Params.bIsRadialDamage)
		{
			EffectContext->SetIsRadialDamage(Params.bIsRadialDamage);
			EffectContext->SetRadialDamageInnerRadius(Params.RadialDamageInnerRadius);
			EffectContext->SetRadialDamageOuterRadius(Params.RadialDamageOuterRadius);
			EffectContext->SetRadialDamageOrigin(Params.RadialDamageOrigin);
		}
	}
	
	Params.SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data, Params.TargetASC);
	return Context;
}

bool UAuraAbilitySystemLibrary::RNGRoll(const float ChanceSuccess)
{
	if (ChanceSuccess >= 100.f) return true;
	const float Roll = FMath::FRandRange(1.f, 100.f);
	return ChanceSuccess >= Roll;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Rotators;
	if (NumRotators <= 1)
	{
		Rotators.Add(Forward.Rotation());
		return Rotators;
	}
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.0f, Axis);
	const float DeltaSpread = Spread / (NumRotators - 1);
	for (int32 i = 0; i < NumRotators; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		Rotators.Add(Direction.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumVectors)
{
	TArray<FVector> Vectors;
	if (NumVectors <= 1)
	{
		Vectors.Add(Forward);
		return Vectors;
	}
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.0f, Axis);
	const float DeltaSpread = Spread / (NumVectors - 1);
	for (int32 i = 0; i < NumVectors; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		Vectors.Add(Direction);
	}
	return Vectors;
}
