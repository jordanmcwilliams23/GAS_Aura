// Copyright Jordan McWilliams


#include "Character/AuraEnemy.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("Attribute Set");

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
	HealthBarWidget->SetupAttachment(GetRootComponent());
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();
}

void AAuraEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnhighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

bool AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	return false;
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* NewCombatTarget)
{
	CombatTarget = NewCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 AAuraEnemy::GetCharacterLevel_Implementation()
{
	return Level;
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController) AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	SpawnLoot();
	Super::Die(DeathImpulse);
}

void AAuraEnemy::ReceivedDamage_Implementation(const float Damage)
{
	if (OnDamageReceived.IsBound())
		OnDamageReceived.Broadcast(Damage);
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//AI should only be run on the server
	if (!HasAuthority()) return;
	AuraAIController = Cast<AAuraAIController>(NewController);

	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	check(AbilitySystemComponent);
	InitAbilityActorInfo();

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}
	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		//Bind HealthChanged to when health attribute changes
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

		//Bind MaxHealthChanged to when max health attribute changes
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		//Hit React Callback
		const FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
		AbilitySystemComponent->RegisterGameplayTagEvent(AuraTags.Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::HitReactTagChanged);
		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, const int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(this);
	const FChampionInfo ChampionInfo = CharacterClassInfo->ChampionInfo;
	if (HasAuthority())
	{
		if (ChampionInfo.bChampionsEnabled && bCanBeChampion && (bForceSpawnAsChampion || RollIsChampion()))
		{
			SetupChampion(ChampionInfo);
		} else
		{
			InitializeDefaultAttributes();
		}
		
		UAuraAbilitySystemLibrary::InitializeDefaultAbilities(this, GetAbilitySystemComponent(), CharacterClass);
	}
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Burn, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::BurnTagChanged);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, GetAbilitySystemComponent());
}

bool AAuraEnemy::RollIsChampion() const
{
	return UAuraAbilitySystemLibrary::RNGRoll(ChanceToBeChampion.GetValueAtLevel(Level));
}

EChampionType AAuraEnemy::GetRandomChampionType()
{
	//return EChampionType::Speedy;
	uint8 RandomInt = FMath::RandRange(0, static_cast<uint8>(EChampionType::Splitter));
	return static_cast<EChampionType>(RandomInt);
}

void AAuraEnemy::DebugTestRandomChampType()
{
	ChampionTypeFrequency.Empty();
	ChampionTypeFrequency.Add(EChampionType::Regenerator, 0);
	ChampionTypeFrequency.Add(EChampionType::Speedy, 0);
	ChampionTypeFrequency.Add(EChampionType::Shooter, 0);
	for (int i = 0; i < 100; ++i)
	{
		ChampionTypeFrequency[GetRandomChampionType()]++;
	}
	for (const TTuple<EChampionType, int32>& Pair : ChampionTypeFrequency)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 4.f, FColor::Emerald, FString::Printf(TEXT("Champion Type: %d Frequency: %d"), Pair.Key, Pair.Value));
	}
}

void AAuraEnemy::Regenerate(const float Damage)
{
	if (!HasAuthority()) return;
	const FChampionInfo ChampionInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(this)->ChampionInfo;
	const UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(GetAttributeSet());
	const float HealthPercentage = 100.f * AuraAttributes->GetHealth() / AuraAttributes->GetMaxHealth();
	/* if (GEngine)
		GEngine->AddOnScreenDebugMessage(39, 5.f, FColor::Green, FString::Printf(TEXT("Enemy Health Percent: %f"), HealthPercentage)); */
	if (HealthPercentage <= ChampionInfo.Regenerator_HealthPercentThreshold)
	{
		const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		const FGameplayEffectSpecHandle Spec = GetAbilitySystemComponent()->MakeOutgoingSpec(ChampionInfo.RegenerationGameplayEffectClass, 1.f, ContextHandle);
		Spec.Data->SetSetByCallerMagnitude(FAuraGameplayTags::Get().Champion_Regenerator_RegenDuration, ChampionInfo.Regenerator_RegenDuration);
		const float RegenAmount = ChampionInfo.Regenerator_RegenPercent / 100.f * AuraAttributes->GetMaxHealth();
		Spec.Data->SetSetByCallerMagnitude(FAuraGameplayTags::Get().Champion_Regenerator_RegenAmount, RegenAmount);
		Spec.Data->Period = ChampionInfo.Regenerator_RegenPeriod;

		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		//Rework this if multiple delegates need to use OnDamageReceived
		this->OnDamageReceived.RemoveAll(this);
	}
}

void AAuraEnemy::ShooterAbility() const
{
	const FChampionInfo ChampionInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(this)->ChampionInfo;
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(ChampionInfo.Shooter_FireProjectileClass);
	GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(Spec);
}

void AAuraEnemy::SetupChampion(const FChampionInfo& ChampionInfo)
{
	bIsChampion = true;
	const float Multiplier = FMath::FRandRange(ChampionInfo.ChampionAttributeMultiplierRange.X, ChampionInfo.ChampionAttributeMultiplierRange.Y);
	UAuraAbilitySystemLibrary::InitializeDefaultAttributesWithMultiplier(this, GetAbilitySystemComponent(), CharacterClass, Level, Multiplier);
	const EChampionType ChosenType = bForceSpawnAsChampion ? ForceChampionType : GetRandomChampionType();
	SetActorScale3D(ChampionInfo.ScaleMultiplier * GetActorScale3D());
			
	UDebuffNiagaraComponent* Particles = NewObject<UDebuffNiagaraComponent>(this);
	Particles->SetupAttachment(GetRootComponent());
	Particles->SetAsset(ChampionInfo.BaseNiagaraSystem);
	//Doesn't really need a debuff tag, filler
	Particles->DebuffTag = FAuraGameplayTags::Get().Champion_Regenerator_RegenAmount;
	Particles->SetAutoDestroy(true);
	Particles->SetupComponent();
	Particles->bAutoActivate = true;
	Particles->RegisterComponent();
	switch (ChosenType)
	{
	case EChampionType::Regenerator:
		OnDamageReceived.AddUObject(this, &AAuraEnemy::Regenerate);
		break;
	case EChampionType::Shooter:
		ShooterAbility();
		break;
	case EChampionType::Speedy:
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * ChampionInfo.Speedy_SpeedMultiplier;
		break;
	case EChampionType::Splitter:
		OnDeath.AddDynamic(this, &AAuraEnemy::Split);
		break;
	default:
		break;
	}
}

