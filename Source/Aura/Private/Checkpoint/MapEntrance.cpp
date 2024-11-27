// Copyright Jordan McWilliams


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Interaction/GameModeInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance()
{
	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	Mesh->SetupAttachment(MoveToComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCustomDepthStencilValue(CustomDepthRenderValue);
	Mesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(MoveToComponent);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMapEntrance::HighlightActor_Implementation()
{
	Mesh->SetRenderCustomDepth(true);
}

void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMapEntrance::OnSphereOverlap);
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(this)))
		{
			GameModeInterface->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		if (DestinationMap == nullptr)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("Map Entrance doesn't have a destination map set!"));
			return;
		}
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
