// Copyright Jordan McWilliams


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerState.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
	for (const FSavedAbility& SavedAbility : SaveData->SavedAbilities)
	{
		const TSubclassOf<UGameplayAbility> StartupAbilityClass = SavedAbility.GameplayAbilityClass;

		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(StartupAbilityClass, SavedAbility.AbilityLevel);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(SavedAbility.AbilitySlot);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(SavedAbility.AbilityStatus);
		if (SavedAbility.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Offensive)
		{
			GiveAbility(LoadedAbilitySpec);
		} else if (SavedAbility.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Passive)
		{
			GiveAbility(LoadedAbilitySpec);
			//Only activate passive ability if it's equipped
			if (SavedAbility.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
			{
				TryActivateAbility(LoadedAbilitySpec.Handle);
			}
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass: Abilities)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1.f);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(Spec.Ability))
		{
			Spec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupGameplayTag);
			Spec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		}
		GiveAbility(Spec);
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass: PassiveAbilities)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1.f);
		GiveAbilityAndActivateOnce(Spec);
		Spec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Tag))
		{
			AbilitySpecInputPressed(Spec);
			if (Spec.IsActive()) {
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}
			return;
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Tag))
		{
			AbilitySpecInputPressed(Spec);
			if (Spec.IsActive()) continue;
			TryActivateAbility(Spec.Handle);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Tag) && Spec.IsActive())
		{
			AbilitySpecInputReleased(Spec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__)
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!AbilitySpec.Ability) return FGameplayTag();
	for (FGameplayTag Tag: 	AbilitySpec.Ability.Get()->AbilityTags)
	{
		if (Tag.MatchesTag(FAuraGameplayTags::Get().Abilities))
			return Tag;
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!AbilitySpec.Ability) return FGameplayTag();
	for (FGameplayTag Tag: 	AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FAuraGameplayTags::Get().InputTag))
			return Tag;
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	const FGameplayTag& StatusAbilityTag = FAuraGameplayTags::Get().Abilities_Status;
	for (const FGameplayTag& Tag: AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(StatusAbilityTag))
			return Tag;
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilityHasSlot(&AbilitySpec, Slot))
			return false;
	}
	return true;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag, const float Amount)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) >= Amount)
		{
			ServerUpgradeAttribute(AttributeTag, Amount);
		}
	}
}

void UAuraAbilitySystemComponent::SetAttribute(const FGameplayTag& AttributeTag, const float Value)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = Value;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag, const float Amount)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = Amount;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddAttributePoints(GetAvatarActor(), -Amount);
	}
}

void UAuraAbilitySystemComponent::SetAttributesBack(const TMap<FGameplayTag, int>& PreviousStatValues, const int PreviousAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(GetOwnerActor());
	const FGameplayEffectContextHandle Context = MakeEffectContext();
	const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(AuraPlayerState->SetPrimaryAttributesGameplayEffect.Get(), 1.f, Context);
	for (const TTuple<FGameplayTag, int>& Pair : PreviousStatValues)
	{
		Spec.Data.Get()->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
	}
	ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	AuraPlayerState->SetAttributePoints(PreviousAttributePoints);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	//Use scoped lock to prevent other functions from making changes to active abilities
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec: ActivatableAbilities.Items)
	{
		for (const FGameplayTag& Tag: AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
				return &AbilitySpec;
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(const int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info: AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid()) continue;
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1.f);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			//Force client replication immediately rather than wait for next update
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddSpellPoints(GetAvatarActor(), -1);
	}
	
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
		FGameplayTag Status = GetStatusTagFromSpec(*AbilitySpec);
		if(Status.MatchesTagExact(AuraGameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags.Abilities_Status_Unlocked);
			Status = AuraGameplayTags.Abilities_Status_Unlocked;
		} else if (Status.MatchesTagExact(AuraGameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(AuraGameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, const int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag PrevInput = GetInputTagFromSpec(*Spec);
		const FGameplayTag Status = GetStatusTagFromSpec(*Spec);
		const FAuraGameplayTags Tags = FAuraGameplayTags::Get();

		//Ability must be unlocked or already equipped to equip again
		//Slot means input here
		const bool bValidStatus = Status == Tags.Abilities_Status_Unlocked || Status == Tags.Abilities_Status_Equipped;
		if (bValidStatus)
		{
			//Handle activation / deactivation for passive abilities

			//Slot is empty?
			if (!SlotIsEmpty(InputTag))
			{
				if (FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(InputTag))
				{
					const FGameplayTag& SlotSpecAbilityTag = GetAbilityTagFromSpec(*SpecWithSlot);
					//If it's the same ability, return early
					if (AbilityTag.MatchesTagExact(SlotSpecAbilityTag))
					{
						ClientEquipAbility(AbilityTag, Tags.Abilities_Status_Equipped, InputTag, PrevInput);
						return;
					}

					if (IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(SlotSpecAbilityTag, false);
						DeactivatePassiveAbility.Broadcast(SlotSpecAbilityTag);
					}
					ClearSlot(SpecWithSlot);
				}
			}

			if (!AbilityHasAnySlot(Spec)) //Ability doesn't yet have a slot (ie. not active yet)
			{
				if (IsPassiveAbility(*Spec))
				{
					MulticastActivatePassiveEffect(AbilityTag, true);
					TryActivateAbility(Spec->Handle);
				}
				Spec->DynamicAbilityTags.RemoveTag(GetStatusTagFromSpec(*Spec));
				Spec->DynamicAbilityTags.AddTag(Tags.Abilities_Status_Equipped);
			}
			AssignSlotToAbility(*Spec, InputTag);
			MarkAbilitySpecDirty(*Spec);
		}
		ClientEquipAbility(AbilityTag, Tags.Abilities_Status_Equipped, InputTag, PrevInput);
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Input, const FGameplayTag& PrevInput)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Input, PrevInput);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	//Remove Input Tag
	Spec->DynamicAbilityTags.RemoveTag(GetInputTagFromSpec(*Spec));
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveAbilitiesLock(*this);
	for (FGameplayAbilitySpec& Spec: ActivatableAbilities.Items)
	{
		if (AbilityHasSlot(&Spec, Slot))
			ClearSlot(&Spec);
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& InputTag)
{
	for (const FGameplayTag& Tag: Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(InputTag)) return true;
	}
	return false;
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec* Spec)
{
	return Spec->DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec: ActivatableAbilities.Items)
	{
		if (Spec.DynamicAbilityTags.HasTagExact(Slot))
			return &Spec;
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
                                                                                const bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                              FString& OutNextDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		OutDescription = UAuraAbilitySystemLibrary::GetAbilityDescription(GetAvatarActor(), AbilityTag, AbilitySpec->Level);
		OutNextDescription = UAuraAbilitySystemLibrary::GetAbilityNextLevelDescription(GetAvatarActor(), AbilityTag, AbilitySpec->Level);
		return true;
	}
	//Failed to find activatable ability ie. must be locked
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	OutNextDescription = FString();
	return false;
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
		return GetStatusTagFromSpec(*Spec);
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (bStartupAbilitiesGiven) return;
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
