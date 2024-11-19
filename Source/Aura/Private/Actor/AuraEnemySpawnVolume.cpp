// Copyright Jordan McWilliams


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Aura/AuraLogChannels.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

// Sets default values
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(Box);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
		Destroy();
}

// Called when the game starts or when spawned
void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;
	bReached = true;

	if (SpawnPoints.IsEmpty())
	{
		UE_LOG(LogAura, Error, TEXT("%s does not have any spawn points set!"), *GetNameSafe(this));
	}
	for (const AAuraEnemySpawnPoint* Point: SpawnPoints)
	{
		if (IsValid(Point))
			Point->SpawnEnemy();
	}
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

