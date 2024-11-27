// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

enum class ECharacterClass : uint8;
class AAuraEnemy;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	AAuraEnemySpawnPoint();

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	int32 EnemyLevel = 1;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	//ECharacterClass CharacterClass = ECharacterClass::Warrior;

	void SetSelected(const bool bSelected) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy Class")
	TObjectPtr<UTexture2D> UnselectedTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy Class")
	TObjectPtr<UTexture2D> SelectedTexture;
};
