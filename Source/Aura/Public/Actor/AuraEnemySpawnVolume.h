// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemySpawnVolume();

	//Save Interface
	virtual void LoadActor_Implementation() override;
	//End Save Interface

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);

	UPROPERTY(EditInstanceOnly)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> SpawnSound;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

	bool bSelected = false;
	
	void HandleOnActorSelectedChanged(const TArray<UObject*>& NewSelection, bool bForceRefresh);
};
