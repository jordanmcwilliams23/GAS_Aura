// Copyright Jordan McWilliams


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, const TSubclassOf<UGameplayEffect> GameplayEffectClass) const
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: InstantGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: DurationGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: InfiniteGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: InstantGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: DurationGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect> Effect: InfiniteGameplayEffectClassArray)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		for (const TSubclassOf<UGameplayEffect> Effect: InfiniteGameplayEffectClassArray)
		{
			TargetASC->RemoveActiveGameplayEffectBySourceEffect(Effect, TargetASC, 1);
		}
	}
}


