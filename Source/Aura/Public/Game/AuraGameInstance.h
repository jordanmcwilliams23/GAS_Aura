// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;
	
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex;

	void LoadedWorld(UWorld* World);

	UPROPERTY()
	FVector InitialMoveToLocation = FVector::ZeroVector;
};
