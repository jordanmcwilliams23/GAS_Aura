// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "Components/TimelineComponent.h"
#include "AuraFireball.generated.h"

class UTimelineComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()
public:
	AAuraFireball();
	
	UPROPERTY()
	TObjectPtr<AActor> ReturnToActor;

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
protected:

	virtual void OnHit() override;

	void SetupTimer();
	
	FVector StartLocation = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Category="Timelines")
	float Distance = 0.f;
	
	FVector Apex = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Category="Timelines")
	float ExplodingDistance = 0.f;

	//Outgoing Timeline
	UPROPERTY()
	TObjectPtr<UTimelineComponent> OutgoingTimeline;

	UPROPERTY(EditDefaultsOnly, Category="Timelines")
	TObjectPtr<UCurveFloat> OutgoingCurve;

	FOnTimelineFloat UpdateOutgoing{};
	FOnTimelineEvent FinishedOutgoing{};

	UFUNCTION()
	void OutgoingUpdate(float Value);

	UFUNCTION()
	void OutgoingFinished();

	//Returning Timeline
	UPROPERTY()
	TObjectPtr<UTimelineComponent> ReturnTimeline;

	UPROPERTY(EditDefaultsOnly, Category="Timelines")
	TObjectPtr<UCurveFloat> ReturnCurve;

	FOnTimelineFloat UpdateReturn{};

	UFUNCTION()
	void ReturnUpdate(float Value);
	
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
};
