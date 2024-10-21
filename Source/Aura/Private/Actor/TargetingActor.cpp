// Copyright Jordan McWilliams


#include "Actor/TargetingActor.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/TargetInterface.h"

ATargetingActor::ATargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetingSphere = CreateDefaultSubobject<USphereComponent>("Targeting Sphere");
	SetRootComponent(TargetingSphere);
	TargetingSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TargetingDecal = CreateDefaultSubobject<UDecalComponent>("Targeting Decal");	
	TargetingDecal->SetupAttachment(GetRootComponent());
}

void ATargetingActor::BeginPlay()
{
	Super::BeginPlay();

	TargetingSphere->OnComponentBeginOverlap.AddDynamic(this, &ATargetingActor::OnTargetingBeginOverlap);
	TargetingSphere->OnComponentEndOverlap.AddDynamic(this, &ATargetingActor::OnTargetingEndOverlap);
}

void ATargetingActor::OnTargetingBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (ITargetInterface* TargetInterface = Cast<ITargetInterface>(OtherActor))
	{
		TargetInterface->HighlightActor();
	}
}

void ATargetingActor::OnTargetingEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ITargetInterface* TargetInterface = Cast<ITargetInterface>(OtherActor))
	{
		TargetInterface->UnhighlightActor();
	}
}

void ATargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATargetingActor::SetTargetingSphereRadius(const float Radius) const
{
	TargetingSphere->SetSphereRadius(Radius);
	TargetingDecal->DecalSize = FVector(Radius);
}

