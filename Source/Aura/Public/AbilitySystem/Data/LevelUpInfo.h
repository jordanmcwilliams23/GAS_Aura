// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT()
struct FLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;
	
	UPROPERTY(EditDefaultsOnly)
	int8 AttributePoints = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int8 SpellPoints = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FLevelInfo> LevelInfo;

	int32 FindLevelForXP(const int32 XP);
	bool CanLevelUp(const int32 Level,const int32 Experience);
	FLevelInfo GetLevelInfo(const int32 Level);
};
