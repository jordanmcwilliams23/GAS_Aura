// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText AttributeName = FText();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FLinearColor AttributeColor = FLinearColor();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayAttribute AttributeGetter;
};

/**
 * 
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAuraAttributeInfo FindAttributeTagInfo(const FGameplayTag& Tag, bool bLogNotFound = false);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation;
};
