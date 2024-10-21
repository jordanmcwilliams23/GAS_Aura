// Copyright Jordan McWilliams


#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePoints.Add(Pt_0);
	SetRootComponent(Pt_0);

	CREATE_AND_SETUP_PT(Pt_1)
	CREATE_AND_SETUP_PT(Pt_2)
	CREATE_AND_SETUP_PT(Pt_3)
	CREATE_AND_SETUP_PT(Pt_4)
	CREATE_AND_SETUP_PT(Pt_5)
	CREATE_AND_SETUP_PT(Pt_6)
	CREATE_AND_SETUP_PT(Pt_7)
	CREATE_AND_SETUP_PT(Pt_8)
	CREATE_AND_SETUP_PT(Pt_9)
	CREATE_AND_SETUP_PT(Pt_10)
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints,
	float YawOverride)
{
	checkf(ImmutablePoints.Num() >= NumPoints, TEXT("Attempted to access immutable points out of bounds"));

	TArray<USceneComponent*> Copy;

	for (USceneComponent* Point: ImmutablePoints)
	{
		if (Copy.Num() > NumPoints) return Copy;
		if (Point != Pt_0)
		{
			FVector ToPoint = Point->GetComponentLocation() - Pt_0->GetComponentLocation()
			.RotateAngleAxis(YawOverride, FVector::UpVector);
			Point->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, Point->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, Point->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, HitResult.ImpactPoint.Z);

		Point->SetWorldLocation(AdjustedLocation);
		Point->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal)); 
		Copy.Add(Point);
	}
	return Copy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


