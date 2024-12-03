// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MapEntranceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UMapEntranceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IMapEntranceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	FVector GetMoveToLocation();
};
