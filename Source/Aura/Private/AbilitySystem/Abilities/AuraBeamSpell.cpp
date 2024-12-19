// Copyright Jordan McWilliams


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.Location;
		MouseHitActor = HitResult.GetActor();
	} else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = OwnerPlayerController->GetCharacter();
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation, const float SphereRadius)
{
	if (!OwnerCharacter || !OwnerCharacter->Implements<UCombatInterface>()) return;
	const USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter);
	if (!Weapon) return;
	const TArray<AActor*> ActorsToIgnore({OwnerCharacter});
	const FVector SocketLocation = Weapon->GetSocketLocation(BeamStartSocketName);
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(
		OwnerCharacter,
		SocketLocation,
		BeamTargetLocation,
		SphereRadius,
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true);
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.Location;
		MouseHitActor = HitResult.GetActor();
		//if MouseHitActor implements combat interface AND PrimaryTargetDied is not yet bound, bind it
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor); CombatInterface && !CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets) const
{
	TArray<AActor*> OverlappingActors;
	const TArray<AActor*> ActorsToIgnore({GetAvatarActorFromActorInfo(), MouseHitActor});
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation());
	const int32 NumAdditionalTargets = BaseTargets + FMath::Min(GetAbilityLevel() - 1, MaxNumTargets);
	UAuraAbilitySystemLibrary::GetClosestTargetsInSight(NumAdditionalTargets, ActorsToIgnore,OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());
	for (AActor* Target : OutAdditionalTargets)
	{
		//if Target implements combat interface AND AdditionalTargetDied is not yet bound, bind it
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target); CombatInterface && !CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
		}
	}
}
