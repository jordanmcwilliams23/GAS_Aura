// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Aura/AuraTypes.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEquippedAbility);
/**
 * 
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable)
	FOnInt32ChangedSignatureDyn SpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquippedAbility SuccessfullyEquippedAbility;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag,UPARAM(ref) FString& OutDescription,UPARAM(ref) FString& OutNextDescription);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const;

	UFUNCTION()
	void OnSpellPointsChanged(const int32 SpellPoints);
};
