// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraAbilitySystemLibrary.generated.h"

class USpellMenuWidgetController;
class AAuraHUD;
class AAuraPlayerController;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class UAbilitySystemComponent;
class AAuraCharacterBase;
struct FGameplayEffectContextHandle;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static bool MakeWidgetControllerParams(const UObject* WorldContext, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContext"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContext"))
	static UAttributeMenuWidgetController* GetAttributeWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContext"))
	static USpellMenuWidgetController* GetSpellWidgetController(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, ECharacterClass Class);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf="WorldContext"))
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContext);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf="WorldContext"))
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContext);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Combat")
	static FVector GetCombatSocketLocation(const AAuraCharacterBase* Character, const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<AAuraPlayerController*> GetAllPlayerControllers(const AActor* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static float GetXPAmount(const UObject* WorldContext, const ECharacterClass Class, const int Level);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary")
	static TArray<FGameplayTag> CallerMagnitudeTags(TSubclassOf<UGameplayEffect> GameplayEffect);

	static FString GetAbilityDescription(const UObject* WorldContextObject, const FGameplayTag& AbilityTag);
	static FString GetAbilityNextLevelDescription(const UObject* WorldContextObject, const FGameplayTag& AbilityTag);
	
	static void FormatAbilityDescriptionAtLevel(UGameplayAbility* Ability, FText& OutDescription, const FGameplayTag& DamageType, int32 Level);
};
