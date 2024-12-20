// Copyright Jordan McWilliams


#include "Actor/TargetingActor.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/HighlightInterface.h"
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

void ATargetingActor::DestroyAfterTime(const float Time)
{
	FTimerHandle DestroyTimerHandle;
	FTimerDelegate DestroyTimerDelegate;
	DestroyTimerDelegate.BindLambda(
		[this]()
		{
			this->Destroy();
		});
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, DestroyTimerDelegate, Time, false);
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
	if (OtherActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(OtherActor);
	}
}

void ATargetingActor::OnTargetingEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnhighlightActor(OtherActor);
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

