// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Aura/Aura.h"
#include "Interaction/HighlightInterface.h"
#include "MapEntrance.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class AURA_API AMapEntrance : public AActor, public IHighlightInterface
{
	GENERATED_BODY()
public:
	AMapEntrance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> MoveToComponent;

	//Highlight Interface
	virtual void HighlightActor_Implementation() override;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag = FName("Dungeon1PlayerStart1");
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& HitResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;
	
	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthRenderValue = CUSTOM_DEPTH_TAN;
};
