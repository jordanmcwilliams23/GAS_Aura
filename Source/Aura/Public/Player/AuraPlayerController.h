// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "Actor/MagicCircle.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/PlayerControllerInterface.h"
#include "AuraPlayerController.generated.h"

class IHighlightInterface;
class UNiagaraSystem;
class UDamageTextComponent;
struct FInputActionInstance;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class UAuraAbilitySystemComponent;
class USplineComponent;

DECLARE_MULTICAST_DELEGATE(FOnReachedDestination);

enum class ETargetingStatus : uint8
{
	TargetingEnemy,
	TargetingNonenemy,
	NotTargeting
};

USTRUCT(BlueprintType)
struct FCameraOccludedActor
{
	GENERATED_USTRUCT_BODY()
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const AActor* Actor = nullptr;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh = nullptr;
	  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UMaterialInterface*> Materials;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsOccluded = false;
};

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController, public IPlayerControllerInterface
{
	GENERATED_BODY()
public:

	/* Player Controller Interface */
	virtual void MenuOpened() override;
	virtual void MenuClosed() override;
	/* End Player Controller Interface */
	
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult GetCursorHit() {return CursorHit;}

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageNumber, AActor* TargetActor,const bool bIsBlockedHit, const bool bICriticalHit);

	UFUNCTION(BlueprintCallable)
	void SyncOccludedActors();
	
	UFUNCTION(BlueprintCallable)
	void ShowTargetingActor(TSubclassOf<ATargetingActor> TargetingActorClass, const bool bInShow, UMaterialInterface* Material, float Radius = 0.f);

	UFUNCTION(BlueprintCallable)
	void BlockInputAndMoveToCachedDestination();

	FOnReachedDestination OnReachedDestination;
	
	FVector CachedDestination = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<ATargetingActor> TargetingActor;

	void SetTargetingActor(ATargetingActor* Target, const float DestroyTime = 1.f)
	{
		if (!Target && TargetingActor)
		{
			TargetingActor->DestroyAfterTime(DestroyTime);
		} 
		TargetingActor = Target;
	}
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	//Occlusion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion|Occlusion",
		meta=(ClampMin="0.1", ClampMax="10.0"))
	float CapsulePercentageForTrace;
	  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion|Materials")
	UMaterialInterface* FadeMaterial;
	 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion|Components")
	class USpringArmComponent* ActiveSpringArm;
	 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion|Components")
	class UCameraComponent* ActiveCamera;
	 
	UPROPERTY(BlueprintReadWrite, Category="Camera Occlusion|Components")
	class UCapsuleComponent* ActiveCapsuleComponent;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion")
	bool IsOcclusionEnabled;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion|Occlusion")
	bool DebugLineTraces;

	int NumMenusOpen = 0;

	void UpdateTargetingStatus();
	
private:

	FInputModeGameAndUI GameAndUIInputMode;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() {bShiftKeyDown = true;}

	void ShiftReleased() {bShiftKeyDown = false;}
	bool bShiftKeyDown = false;
	
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	void Move(const struct FInputActionValue& InputActionValue);
	void CursorTrace();
	
	TWeakObjectPtr<AActor> LastActor;
	TWeakObjectPtr<AActor> CurrentActor;

	static void HighlightActor(AActor* InActor);
	static void UnhighlightActor(AActor* InActor);


	void AbilityInputTagPressed(const FInputActionInstance& Instance, FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(const FInputActionValue& Value, FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	void AutoRun();
	
	FHitResult CursorHit;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	TMap<const AActor*, FCameraOccludedActor> OccludedActors;

	bool HideOccludedActor(const AActor* Actor);
	void OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const;
	void ShowOccludedActor(FCameraOccludedActor& OccludedActor);
	static void OnShowOccludedActor(const FCameraOccludedActor& OccludedActor);
	void ForceShowOccludedActors();
	 
	__forceinline bool ShouldCheckCameraOcclusion() const
	{
		return IsOcclusionEnabled && FadeMaterial && ActiveCamera && ActiveCapsuleComponent;
	}

	void UpdateMagicCircleLocation() const;

	void EnableControls();

	FDelegateHandle EnableControlsDelegateHandle;
};
