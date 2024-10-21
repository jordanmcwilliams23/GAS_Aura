// Copyright Jordan McWilliams


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoomComponent->SetupAttachment(GetRootComponent());
	CameraBoomComponent->SetUsingAbsoluteRotation(true);
	CameraBoomComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 400.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

//Server
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

//Client
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetCharacterLevel_Implementation()
{
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(const int32 InXP)
{
	check(AuraPlayerState);
	AuraPlayerState->AddXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

void AAuraCharacter::AddAttributePoints_Implementation(int32 Points)
{
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoints(Points);
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::AddSpellPoints_Implementation(const int32 Points)
{
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoints(Points);
}

void AAuraCharacter::ShowTargetingActor_Implementation(const TSubclassOf<ATargetingActor> TargetingActorSubclass, const bool bInShow, UMaterialInterface* Material, float Radius)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowTargetingActor(TargetingActorSubclass, bInShow, Material, Radius);
		AuraPlayerController->bShowMouseCursor = !bInShow;
	}
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		//Rotate level up particles to face camera
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		const FGameplayTagContainer BlockedTags = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({
				AuraTags.Player_Block_CursorTrace,
				AuraTags.Player_Block_InputHeld,
				AuraTags.Player_Block_InputPressed,
				AuraTags.Player_Block_InputReleased}));
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
		} else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
		}
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 Level)
	{
		Execute_LevelUp(this);
	});
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AttributeSet = AuraPlayerState->GetAttributeSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Burn, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::BurnTagChanged);
	
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);

	//Only valid on local player
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}

