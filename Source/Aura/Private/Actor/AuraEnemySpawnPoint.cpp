// Copyright Jordan McWilliams


#include "Actor/AuraEnemySpawnPoint.h"

#include "Aura/AuraLogChannels.h"
#include "Character/AuraEnemy.h"
#include "Components/BillboardComponent.h"

AAuraEnemySpawnPoint::AAuraEnemySpawnPoint()
{
	if (UnselectedTexture)
		GetSpriteComponent()->SetSprite(UnselectedTexture);
}

void AAuraEnemySpawnPoint::SpawnEnemy() const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (EnemyClass == nullptr)
	{
		UE_LOG(LogAura, Error, TEXT("%s does not have enemy class set!"), *GetNameSafe(this));
		return;
	}
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	//Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}

void AAuraEnemySpawnPoint::SetSelected(const bool bSelected) const
{
	if (IsValid(GetSpriteComponent()))
		GetSpriteComponent()->SetSprite(bSelected ? SelectedTexture : UnselectedTexture);
}
