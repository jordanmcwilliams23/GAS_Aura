// Copyright Jordan McWilliams


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Interaction/GameModeInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(this)))
		{
			GameModeInterface->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void AMapEntrance::LoadActor_Implementation()
{
	//Do nothing when loading map entrance
}
