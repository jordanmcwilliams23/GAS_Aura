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
	const AActor* Actor;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;
	  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UMaterialInterface*> Materials;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsOccluded;
};

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController, public IPlayerControllerInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult GetCursorHit() {return CursorHit;}

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageNumber, AActor* TargetActor,const bool bIsBlockedHit, const bool bICriticalHit);

	UFUNCTION(BlueprintCallable)
	void SyncOccludedActors();

	/* Player Controller Interface */
	virtual void MenuOpened() override;
	virtual void MenuClosed() override;
	/* End Player Controller Interface */

	UFUNCTION(BlueprintCallable)
	void ShowTargetingActor(TSubclassOf<ATargetingActor> TargetingActorClass, const bool bInShow, UMaterialInterface* Material, float Radius = 0.f);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	//Occlusion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Occlusion|Occlusion",
		meta=(ClampMin="0.1", ClampMax="10.0") )
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

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	UPROPERTY()
	TObjectPtr<ATargetingActor> TargetingActor;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	void Move(const struct FInputActionValue& InputActionValue);
	void CursorTrace();
	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> CurrentActor;

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
};
