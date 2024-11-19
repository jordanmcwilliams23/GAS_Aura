// Copyright Jordan McWilliams


#include "Checkpoint/Checkpoint.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/GameModeInterface.h"
#include "Interaction/PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->bDrawOnlyIfSelected = false;
	
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthRenderValue);
	CheckpointMesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

bool ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
	return true;
}

void ACheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void ACheckpoint::UnhighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		if (const IGameModeInterface* GameModeInterface = Cast<IGameModeInterface>(UGameplayStatics::GetGameMode(this)))
		{
			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);
			
			GameModeInterface->SaveWorldState(GetWorld(), MapName);
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		HandleGlowEffects();
	}
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, MaterialInstanceDynamic);
	CheckpointReached(MaterialInstanceDynamic);
}
