// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "TargetingActor.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class AURA_API AMagicCircle : public ATargetingActor
{
	GENERATED_BODY()
	
public:	
	AMagicCircle();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
};
