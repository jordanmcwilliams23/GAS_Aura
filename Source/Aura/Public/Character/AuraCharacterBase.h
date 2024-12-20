
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/AuraTypes.h"
#include "Interaction/CombatInterface.h"
#include "GameplayTagContainer.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	virtual void Die(const FVector& DeathImpulse) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	FOnFloatChangedSignature OnDamageDelegate;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_Burning, BlueprintReadOnly)
	bool bIsBurning = false;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsBeingShocked = false;

	UFUNCTION()
	virtual void OnRep_Stunned();
	
	UFUNCTION()
	virtual void OnRep_Burning();

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;

	void SetCharacterClass(const ECharacterClass InClass) { CharacterClass = InClass; }
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	/* Combat Interface */
	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& Tag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void IncrementMinionCount_Implementation(const int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual void SetBeingShocked_Implementation(bool InBeingShocked) override;
	virtual FOnFloatChangedSignature& GetOnDamageSignature() override;
	virtual int GetCCStacks_Implementation() const override;
	/* End Combat Interface */
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FOnASCRegistered OnASCRegistered;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Effect, float Level) const;

	void AddCharacterAbilities() const;

	UFUNCTION()
	virtual void InitializeDefaultAttributes() const;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	/* Dissolve Effects */

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* EnemyDissolveDMI,UMaterialInstanceDynamic* WeaponDissolveDMI);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	/* Minions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinionCount = 0;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> BurnNiagaraSystemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> StunNiagaraSystemClass;

	UPROPERTY(BlueprintReadWrite)
	int CrowdControlStacks = 0;

	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category="Abilities")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TSubclassOf<UGameplayEffect> DeathGameplayEffectClass;
};
