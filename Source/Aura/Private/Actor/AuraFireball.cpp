// Copyright Jordan McWilliams


#include "Actor/AuraFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAuraFireball::AAuraFireball()
{
	OutgoingTimeline = CreateDefaultSubobject<UTimelineComponent>("OutgoingTimeline");
	UpdateOutgoing.BindUFunction(this, FName("OutgoingUpdate"));
	FinishedOutgoing.BindUFunction(this, FName("OutgoingFinished"));

	ReturnTimeline = CreateDefaultSubobject<UTimelineComponent>("ReturnTimeline");
	UpdateReturn.BindUFunction(this, FName("ReturnUpdate"));
}

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetupTimer();
		OutgoingTimeline->Play();
	}
}

void AAuraFireball::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	bHit = true;
}

void AAuraFireball::SetupTimer()
{
	if (OutgoingCurve && ReturnCurve)
	{
		OutgoingTimeline->AddInterpFloat(OutgoingCurve, UpdateOutgoing, FName("Alpha"));
		OutgoingTimeline->SetTimelineFinishedFunc(FinishedOutgoing);
		OutgoingTimeline->SetLooping(false);
		OutgoingTimeline->SetIgnoreTimeDilation(true);

		ReturnTimeline->AddInterpFloat(ReturnCurve, UpdateReturn, FName("Alpha"));
		ReturnTimeline->SetLooping(false);
		ReturnTimeline->SetIgnoreTimeDilation(true);
	}
	StartLocation = GetActorLocation();
	Apex = StartLocation + (GetActorForwardVector() * Distance);
}

void AAuraFireball::OutgoingUpdate(float Value)
{
	const FVector LerpLocation = UKismetMathLibrary::VLerp(StartLocation, Apex, Value);
	SetActorLocation(LerpLocation);
}

void AAuraFireball::OutgoingFinished()
{
	EmptyIgnoreList();
	ReturnTimeline->Play();
}

void AAuraFireball::ReturnUpdate(float Value)
{
	if (IsValid(ReturnToActor))
	{
		const FVector LerpLocation = UKismetMathLibrary::VLerp(Apex, ReturnToActor->GetActorLocation(), Value);
		SetActorLocation(LerpLocation);
		if (GetDistanceTo(ReturnToActor) <= ExplodingDistance)
		{
			OnHit();
			ExplosionDamageParams.bIsRadialDamage = true;
			ExplosionDamageParams.RadialDamageOrigin = GetActorLocation();
			ExplosionDamageParams.KnockbackChance = 100.f;
			TArray<AActor*> OverlappingActors;
			TArray<AActor*> ActorsToIgnore({GetInstigator(), this});
			UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
				this,
				OverlappingActors,
				ActorsToIgnore,
				ExplosionDamageParams.RadialDamageOuterRadius,
				GetActorLocation()
				);
			/* DrawDebugSphere(
				GetWorld(),
				GetActorLocation(),
				ExplosionDamageParams.RadialDamageOuterRadius,
				16,
				FColor::Red,
				false,
				3.f
				); */
			const FVector CurrLocation = GetActorLocation();
			for (AActor*& Overlapped: OverlappingActors)
			{
				FVector Direction = (Overlapped->GetActorLocation() - CurrLocation).GetSafeNormal();
				ExplosionDamageParams.KnockbackForce = Direction * 2000.f;
				ExplosionDamageParams.DeathImpulse = Direction * 1000.f;
				ExplosionDamageParams.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Overlapped);
				UAuraAbilitySystemLibrary::ApplyDamageEffect(ExplosionDamageParams);
			}
			Destroy();
		}
	} else
	{
		OnHit();
		Destroy();
	}
}

void AAuraFireball::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if(!IsValidOverlap(OtherActor)) return;
	if (HasAuthority())
	{
		IgnoreList.Add(OtherActor);
		if (UAbilitySystemComponent* OverlappedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetASC = OverlappedASC;
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.GetDeathImpulseMagnitude();
			DamageEffectParams.DeathImpulse = DeathImpulse;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
