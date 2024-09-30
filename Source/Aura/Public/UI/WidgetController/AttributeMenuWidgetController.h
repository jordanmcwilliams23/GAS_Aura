// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Aura/AuraTypes.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;
struct FAuraAttributeInfo;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, info);
/**
 * 
 */
UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnInt32ChangedSignatureDyn AttributePointsChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnInt32ChangedSignatureDyn SpellPointsChangedDelegate;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag, const float Amount);

	UFUNCTION(BlueprintCallable)
	void SetAttributesBack(const TMap<FGameplayTag, int>& PreviousStatValues, int PreviousAttributePoints);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
private:
	void BroadcastAttributeInfo(const FGameplayTag& Tag) const;
};
