// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetingActor.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS(BlueprintType)
class AURA_API ATargetingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetingActor();
	virtual void Tick(float DeltaTime) override;

	void SetTargetingSphereRadius(const float Radius) const;

	FORCEINLINE UDecalComponent* GetDecalComponent() const { return TargetingDecal; }

	void DestroyAfterTime(const float Time = 0.f);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> TargetingSphere;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDecalComponent> TargetingDecal;

	UFUNCTION()
	void OnTargetingBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTargetingEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
