#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC;
	
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector2f DeathImpulseMagnitudeMinMax = FVector2f::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldHitReact = true;

	UPROPERTY(BlueprintReadWrite)
	float KnockbackMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;
	
	float GetDeathImpulseMagnitude() const { return FMath::RandRange(DeathImpulseMagnitudeMinMax.X, DeathImpulseMagnitudeMinMax.Y); }
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetCriticalHit(const bool IsCriticalHit) { bIsCriticalHit = IsCriticalHit; }
	void SetBlockedHit(const bool IsBlockedHit) { bIsBlockedHit = IsBlockedHit; }

	bool AppliedDebuff() const { return bAppliedDebuff;}
	float GetDebuffDamage() const { return DebuffDamage;}
	float GetDebuffDuration() const { return DebuffDuration;}
	float GetDebuffFrequency() const { return DebuffFrequency;}
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType;}
	FVector GetDeathImpulse() const { return DeathImpulse; }
	bool GetShouldHitReact() const { return bShouldHitReact; }
	FVector GetKnockbackForce() const { return KnockbackForce; }
	float GetKnockbackChance() const { return KnockbackChance; }

	void SetAppliedDebuff(const bool InAppliedDebuff) {bAppliedDebuff = InAppliedDebuff;}
	void SetDebuffDamage(const float InDebuffDamage) {DebuffDamage = InDebuffDamage;}
	void SetDebuffFrequency(const float InDebuffFrequency) {DebuffFrequency = InDebuffFrequency;}
	void SetDebuffDuration(const float InDebuffDuration) {DebuffDuration = InDebuffDuration;}
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) {DamageType = InDamageType;}
	void SetDeathImpulse(const FVector& InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	void SetShouldHitReact(const bool InShouldHitReact) { bShouldHitReact = InShouldHitReact; }
	void SetKnockbackForce(const FVector& InKnockbackForce) { KnockbackForce = InKnockbackForce; }
	void SetKnockbackChance(const float InKnockbackChance) { KnockbackChance = InKnockbackChance; }
	

	
	virtual UScriptStruct* GetScriptStruct() const override;

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bAppliedDebuff = false;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;

	TSharedPtr<FGameplayTag> DamageType;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	bool bShouldHitReact = true;

	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY()
	float KnockbackChance = 0.f;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true, 
		WithCopy = true
	};
};