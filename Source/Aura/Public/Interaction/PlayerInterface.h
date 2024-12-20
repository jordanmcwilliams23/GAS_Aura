// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class ATargetingActor;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	void AddAttributePoints(int32 Points);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent)
	void AddSpellPoints(int32 Points);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DefaultToSelf="Target"))
	ATargetingActor* ShowTargetingActor(TSubclassOf<ATargetingActor> TargetingActorSubclass, const bool bInShow, UMaterialInterface* Material = nullptr, float Radius = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DefaultToSelf="Target"))
	void SaveProgress(const FName& CheckpointTag);
};
