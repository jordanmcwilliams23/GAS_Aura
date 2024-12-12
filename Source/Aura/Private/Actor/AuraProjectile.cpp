// Copyright Jordan McWilliams


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::EmptyIgnoreList()
{
	IgnoreList.Empty();
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	UGameplayStatics::SpawnSoundAttached(
		LoopingSpawnSound,
		GetRootComponent(),
		NAME_None,
		FVector(ForceInit),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true);
	SetLifeSpan(LifeSpan);
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if(!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();

	if (HasAuthority())
	{
		if (bRadialDamage)
		{
			TArray<AActor*> OverlappingActors;

			IgnoreList.Empty();
			IgnoreList.Add(GetOwner());
			FVector Location = GetActorLocation();
			UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
				this,
				OverlappingActors,
				IgnoreList,
				RadialDamageOuterRadius,
				Location);
			for (AActor* Overlapped : OverlappingActors)
			{
				if (UAbilitySystemComponent* OverlappedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Overlapped))
				{
					DamageEffectParams.TargetASC = OverlappedASC;
					DamageEffectParams.RadialDamageOrigin = Location;
					const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.GetDeathImpulseMagnitude();
					DamageEffectParams.DeathImpulse = DeathImpulse;
					//Check for knockback
					if (UAuraAbilitySystemLibrary::RNGRoll(DamageEffectParams.KnockbackChance))
					{
						FRotator Rotation = GetActorRotation();
						Rotation.Pitch = 45.f;
						DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackMagnitude;
					}
					UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
				}
			}
		} else
		{
			if (UAbilitySystemComponent* OverlappedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				DamageEffectParams.TargetASC = OverlappedASC;
				const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.GetDeathImpulseMagnitude();
				DamageEffectParams.DeathImpulse = DeathImpulse;
				//Check for knockback
				if (UAuraAbilitySystemLibrary::RNGRoll(DamageEffectParams.KnockbackChance))
				{
					FRotator Rotation = GetActorRotation();
					Rotation.Pitch = 45.f;
					DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackMagnitude;
				}
				UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
			}
		}
		Destroy();
	} else { bHit = true; }
}

bool AAuraProjectile::IsValidOverlap(const AActor* OtherActor) const
{
	//Don't hit owning actor, friendly actors, or ignored
	return OtherActor != GetOwner() && UAuraAbilitySystemLibrary::IsNotFriend(GetOwner(), OtherActor) && !IgnoreList.Contains(OtherActor);
}

