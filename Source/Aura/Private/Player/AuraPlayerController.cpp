// Copyright Jordan McWilliams


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/TargetInterface.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Containers/Set.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	UpdateMagicCircleLocation();

	if (!bAutoRunning) return;
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float DamageNumber, AActor* TargetActor, const bool bIsBlockedHit, const bool bIsCriticalHit)
{
	if (IsValid(TargetActor) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetActor, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->SetDamageText(DamageNumber, bIsBlockedHit, bIsCriticalHit);
		DamageTextComponent->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	GameAndUIInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GameAndUIInputMode.SetHideCursorDuringCapture(false);
	SetInputMode(GameAndUIInputMode);
	
	if (IsValid(GetPawn()))
	{
		ActiveSpringArm = Cast<
		  USpringArmComponent>(GetPawn()->GetComponentByClass(USpringArmComponent::StaticClass()));
		ActiveCamera = Cast<UCameraComponent>(GetPawn()->GetComponentByClass(UCameraComponent::StaticClass()));
		ActiveCapsuleComponent = Cast<UCapsuleComponent>(
		  GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass()));
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(AuraInputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;
	const FVector2d FInputVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, FInputVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, FInputVector.X);
		bAutoRunning = false;
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor) LastActor->UnhighlightActor();
		if (CurrentActor) CurrentActor->UnhighlightActor();
		LastActor = nullptr;
		CurrentActor = nullptr;
		return;
	}
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	LastActor = CurrentActor;
	CurrentActor = CursorHit.GetActor();
	if (LastActor != CurrentActor)
	{
		if (LastActor) LastActor->UnhighlightActor();
		if (CurrentActor) CurrentActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FInputActionInstance& Instance, const FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;
	bTargeting = CurrentActor != nullptr;
	bAutoRunning = false;
	if (GetASC())
	{
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased)) return;
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) && GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetASC()) { GetASC()->AbilityInputTagReleased(InputTag); }
	
	if (bTargeting || bShiftKeyDown) return;
	
	if (const APawn* ControlledPawn = GetPawn(); FollowTime <= ShortPressThreshold && ControlledPawn)
	{
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
		{
			Spline->ClearSplinePoints();
			for (const FVector& Point: NavPath->PathPoints) Spline->AddSplinePoint(Point, ESplineCoordinateSpace::World);
			
			if (NavPath->PathPoints.Num() > 0)
			{
				CachedDestination = NavPath->PathPoints.Last();
				bAutoRunning = true;
			}
		}
		if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		}
	}
	FollowTime = 0.f;
	bTargeting = false;
}

void AAuraPlayerController::AbilityInputTagHeld(const FInputActionValue& Value, const FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) return;
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) && GetASC())
	{
		GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	} else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		//Move in CachedDestination's direction
		if (APawn* ControlledPawn = GetPawn()) ControlledPawn->AddMovementInput((CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal(), 1.f);
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr) AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		if (const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length(); DistanceToDestination <= AutoRunAcceptanceRadius) bAutoRunning = false;
	}
}

void AAuraPlayerController::SyncOccludedActors()
	{
	  if (!ShouldCheckCameraOcclusion()) return;
	 
	  // Camera is currently colliding, show all current occluded actors
	  // and do not perform further occlusion
	  if (ActiveSpringArm->bDoCollisionTest)
	  {
	    ForceShowOccludedActors();
	    return;
	  }
	 
	  FVector Start = ActiveCamera->GetComponentLocation();
	  FVector End = GetPawn()->GetActorLocation() + FVector(0.f, 0.f, ActiveCapsuleComponent->GetScaledCapsuleHalfHeight());
	 
	  TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;
	  CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	 
	  TArray<AActor*> ActorsToIgnore; // TODO: Add configuration to ignore actor types
	  TArray<FHitResult> OutHits;
	 
	  auto ShouldDebug = DebugLineTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	 
	  bool bGotHits = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
	    GetWorld(), Start, End, ActiveCapsuleComponent->GetScaledCapsuleRadius() * CapsulePercentageForTrace,
	    ActiveCapsuleComponent->GetScaledCapsuleHalfHeight() * CapsulePercentageForTrace, CollisionObjectTypes, true,
	    ActorsToIgnore,
	    ShouldDebug,
	    OutHits, true);
	 
	  if (bGotHits)
	  {
	    // The list of actors hit by the line trace, that means that they are occluded from view
	    TSet<const AActor*> ActorsJustOccluded;
	 
	    // Hide actors that are occluded by the camera
	    for (FHitResult Hit : OutHits)
	    {
	      HideOccludedActor(Hit.GetActor());
	      ActorsJustOccluded.Add(Hit.GetActor());
	    }
	 
	    // Show actors that are currently hidden but that are not occluded by the camera anymore 
	    for (auto& Elem : OccludedActors)
	    {
	      if (!ActorsJustOccluded.Contains(Elem.Value.Actor) && Elem.Value.IsOccluded)
	      {
	        ShowOccludedActor(Elem.Value);
	 
	        if (DebugLineTraces)
	        {
	          UE_LOG(LogTemp, Warning,
	                 TEXT("Actor %s was occluded, but it's not occluded anymore with the new hits."), *Elem.Value.Actor->GetName());
	        }
	      }
	    }
	  }
	  else
	  {
	    ForceShowOccludedActors();
	  }
	}

void AAuraPlayerController::MenuOpened()
{
	if (++NumMenusOpen == 1)
		SetInputMode(FInputModeUIOnly());
}

void AAuraPlayerController::MenuClosed()
{
	if (--NumMenusOpen == 0)
		SetInputMode(GameAndUIInputMode);
}

void AAuraPlayerController::ShowTargetingActor(TSubclassOf<ATargetingActor> TargetingActorClass, const bool bInShow, UMaterialInterface* Material, float Radius)
{
	if (bInShow && !IsValid(TargetingActor))
	{
		const FVector TargetingActorLocation = CursorHit.ImpactPoint;
		TargetingActor = GetWorld()->SpawnActor<ATargetingActor>(TargetingActorClass, TargetingActorLocation, FRotator::ZeroRotator);
		if (Material)
			TargetingActor->GetDecalComponent()->SetMaterial(0, Material);
		if (Radius != 0.f)
			TargetingActor->SetTargetingSphereRadius(Radius);
	} else if (IsValid(TargetingActor))
		TargetingActor->Destroy();
}

bool AAuraPlayerController::HideOccludedActor(const AActor* Actor)
{
  FCameraOccludedActor* ExistingOccludedActor = OccludedActors.Find(Actor);
 
  if (ExistingOccludedActor && ExistingOccludedActor->IsOccluded)
  {
    if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was already occluded. Ignoring."),
                                *Actor->GetName());
    return false;
  }
 
  if (ExistingOccludedActor && IsValid(ExistingOccludedActor->Actor))
  {
    ExistingOccludedActor->IsOccluded = true;
    OnHideOccludedActor(*ExistingOccludedActor);
 
    if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s exists, but was not occluded. Occluding it now."), *Actor->GetName());
  }
  else
  {
    UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(
      Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
 
    FCameraOccludedActor OccludedActor;
    OccludedActor.Actor = Actor;
    OccludedActor.StaticMesh = StaticMesh;
    OccludedActor.Materials = StaticMesh->GetMaterials();
    OccludedActor.IsOccluded = true;
    OccludedActors.Add(Actor, OccludedActor);
    OnHideOccludedActor(OccludedActor);
 
    if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s does not exist, creating and occluding it now."), *Actor->GetName());
  }
	return true;
}
	 
	 
void AAuraPlayerController::ForceShowOccludedActors()
	{
	  for (auto& Elem : OccludedActors)
	  {
	    if (Elem.Value.IsOccluded)
	    {
	      ShowOccludedActor(Elem.Value);
	 
	      if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was occluded, force to show again."), *Elem.Value.Actor->GetName());
	    }
	  }
	}

void AAuraPlayerController::UpdateMagicCircleLocation() const
{
	if (IsValid(TargetingActor))
	{
		if (FHitResult HitResult; GetHitResultUnderCursor(ECC_IgnorePawns, false, HitResult))
		{
			TargetingActor->SetActorLocation(HitResult.Location);
			TargetingActor->SetActorRotation(HitResult.Normal.Rotation());
		}
	}
}

void AAuraPlayerController::ShowOccludedActor(FCameraOccludedActor& OccludedActor)
	{
	  if (!IsValid(OccludedActor.Actor))
	  {
	    OccludedActors.Remove(OccludedActor.Actor);
	  }
	 
	  OccludedActor.IsOccluded = false;
	  OnShowOccludedActor(OccludedActor);
	}
	 
void AAuraPlayerController::OnShowOccludedActor(const FCameraOccludedActor& OccludedActor)
{
	for (int MatIdx = 0; MatIdx < OccludedActor.Materials.Num(); ++MatIdx)
	{
		OccludedActor.StaticMesh->SetMaterial(MatIdx, OccludedActor.Materials[MatIdx]);
	}
	OccludedActor.StaticMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
}
	 
void AAuraPlayerController::OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	for (int i = 0; i < OccludedActor.StaticMesh->GetNumMaterials(); ++i)
	{
		OccludedActor.StaticMesh->SetMaterial(i, FadeMaterial);
	}
	OccludedActor.StaticMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
}

