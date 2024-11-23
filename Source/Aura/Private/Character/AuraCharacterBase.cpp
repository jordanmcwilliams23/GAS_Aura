// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Prevent camera from clipping into capsule or mesh
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetUsingAbsoluteRotation(true);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurning);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const { return AbilitySystemComponent; }

void AAuraCharacterBase::OnRep_Stunned()
{
	//Needed so stun particles replicate to client
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(AuraTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(AuraTags.Player_Block_InputHeld);
		BlockedTags.AddTag(AuraTags.Player_Block_InputPressed);
		BlockedTags.AddTag(AuraTags.Player_Block_InputReleased);
		UDebuffNiagaraComponent* StunDebuffComponent = NewObject<UDebuffNiagaraComponent>(this);
		StunDebuffComponent->SetAsset(StunNiagaraSystemClass);
		StunDebuffComponent->SetupAttachment(GetRootComponent());
		StunDebuffComponent->RegisterComponent();
		StunDebuffComponent->SetupComponent();
		StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		} else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacterBase::OnRep_Burning()
{
	
	if (bIsBurning && !GetAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Debuff_Burn))
	{
		UDebuffNiagaraComponent* BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
		BurnDebuffComponent->SetupAttachment(GetRootComponent());
		BurnDebuffComponent->RegisterComponent();
		BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
		BurnDebuffComponent->Activate();
	} else
	{
		//BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bPreviouslyBurning = bIsBurning;
	bIsBurning = NewCount > 0;
	if (bIsBurning && !bPreviouslyBurning)
	{
		UDebuffNiagaraComponent* BurnDebuffComponent = NewObject<UDebuffNiagaraComponent>(this);
		BurnDebuffComponent->SetupAttachment(GetRootComponent());
		BurnDebuffComponent->SetAsset(BurnNiagaraSystemClass);
		BurnDebuffComponent->RegisterComponent();
		BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
		BurnDebuffComponent->SetupComponent();
		BurnDebuffComponent->Activate();
	}
	else
	{
		//BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacterBase::BeginPlay() { Super::BeginPlay(); }

void AAuraCharacterBase::InitAbilityActorInfo(){}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation() const { return HitReactMontage; }

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	bIsDead = true;

	GetAbilitySystemComponent()->AddReplicatedLooseGameplayTag(FAuraGameplayTags::Get().Status_Dead);
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	OnDeath.Broadcast(this);
}

float AAuraCharacterBase::TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
									 AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& Tag) { return UAuraAbilitySystemLibrary::GetCombatSocketLocation(this, Tag); }

bool AAuraCharacterBase::IsDead_Implementation() const { return bIsDead; }

AActor* AAuraCharacterBase::GetAvatar_Implementation() { return this; }

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation() { return AttackMontages; }

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage& TaggedMontage: AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
			return TaggedMontage;
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation() const { return MinionCount; }

void AAuraCharacterBase::IncrementMinionCount_Implementation(const int32 Amount) { MinionCount += Amount; }

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation() { return CharacterClass; }

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate() { return OnASCRegistered; }

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate() { return OnDeath; }

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation() const { return Weapon; }

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::SetBeingShocked_Implementation(const bool InBeingShocked)
{
	bIsBeingShocked = InBeingShocked;
}

FOnFloatChangedSignature& AAuraCharacterBase::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> Effect, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(Effect);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle Spec = GetAbilitySystemComponent()->MakeOutgoingSpec(Effect, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority()) return;
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation() { return BloodSystem; }

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance) && IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DissolveDMI = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DissolveDMI);
		
		UMaterialInstanceDynamic* WeaponDissolveDMI = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, WeaponDissolveDMI);
		
		StartDissolveTimeline(DissolveDMI, WeaponDissolveDMI);
	}
}

