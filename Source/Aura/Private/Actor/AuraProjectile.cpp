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
	//Don't hit owning actor or actors on same team
	if (OtherActor == GetOwner() || !UAuraAbilitySystemLibrary::IsNotFriend(GetOwner(), OtherActor)) return;
	if (!bHit) OnHit();

	if (HasAuthority())
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
		Destroy();
	} else { bHit = true; }
}

