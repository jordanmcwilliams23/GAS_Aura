// Copyright Jordan McWilliams


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

//Raw struct for internal C++ use here, notice no 'F' prefix or USTRUCT()
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	friend static AuraDamageStatics& DamageStatics();
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetTagsToCaptureDef()
	{
		if (TagsToCaptureDefs.IsEmpty()) InitMap();
		return TagsToCaptureDefs;
	}
private:
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}

	void InitMap()
	{
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

FORCEINLINE static AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::HandleDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& OwningSpec, const FAggregatorEvaluateParameters& EvaluateParameters)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	const FGameplayTag& DamageType = UAuraAbilitySystemLibrary::GetDamageType(OwningSpec.GetContext());
	
	const float DebuffChance = OwningSpec.GetSetByCallerMagnitude(Tags.Debuff_Chance, false, -1.f);
	float TargetDebuffResistance = 0.f;
	const FGameplayTag& ResistanceTag = Tags.DamageTypesToResistances[DamageType];
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().GetTagsToCaptureDef()[ResistanceTag], EvaluateParameters, TargetDebuffResistance);
	TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);
	const float EffectiveDebuffChance = DebuffChance * (100.f - TargetDebuffResistance) / 100.f;
	//Was debuff applied?
	if (UAuraAbilitySystemLibrary::RNGRoll(EffectiveDebuffChance))
	{
		FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(OwningSpec.GetContext().Get());
		const float DebuffDamage = OwningSpec.GetSetByCallerMagnitude(Tags.Debuff_Damage, false, -1.f);
		const float DebuffFrequency = OwningSpec.GetSetByCallerMagnitude(Tags.Debuff_Frequency, false, -1.f);
		const float DebuffDuration = OwningSpec.GetSetByCallerMagnitude(Tags.Debuff_Duration, false, -1.f);
		AuraEffectContext->SetAppliedDebuff(true);
		AuraEffectContext->SetDamageType(MakeShared<FGameplayTag>(DamageType));
		AuraEffectContext->SetDebuffDamage(DebuffDamage);
		AuraEffectContext->SetDebuffDuration(DebuffDuration);
		AuraEffectContext->SetDebuffFrequency(DebuffFrequency);
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourceLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
		SourceLevel = ICombatInterface::Execute_GetCharacterLevel(SourceAvatar);
		
	int32 TargetLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
		TargetLevel = ICombatInterface::Execute_GetCharacterLevel(TargetAvatar);

	const FGameplayEffectSpec& OwningSpec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = OwningSpec.GetContext();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = OwningSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = OwningSpec.CapturedTargetTags.GetAggregatedTags();

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	HandleDebuff(ExecutionParams, OwningSpec, EvaluateParameters);
	
	const FGameplayTag DamageTypeTag = UAuraAbilitySystemLibrary::GetDamageType(EffectContextHandle);
	const FGameplayTag ResistanceTag = Tags.DamageTypesToResistances[DamageTypeTag];
	const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().GetTagsToCaptureDef()[ResistanceTag];

	//Get Damage Set by caller
	float Damage = OwningSpec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
	if (Damage <= 0.f) return;
	
	float Resistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
	Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

	Damage *= 1.f - Resistance/100.f;

	//check radial damage
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()); EffectContext->GetIsRadialDamage())
	{
		Damage = CalculateRadialDamage(EffectContextHandle, Damage, TargetAvatar);
	}
	
	//Capture BlockChance on target and determine if attack was blocked
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Clamp(TargetBlockChance, 0.f, TargetBlockChance);
	//If block, half the damage
	if (const float RNG = FMath::RandRange(1, 100); TargetBlockChance > RNG)
	{
		Damage /= 2.f;
		UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, true);
	}
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Clamp(TargetArmor, 0.f, TargetArmor);

	const FRealCurve* ArmorPenCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	float SourceArmorPen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPen);
	SourceArmorPen = FMath::Clamp(TargetArmor, 0.f, SourceArmorPen);
	const float ArmorPenCoefficient = ArmorPenCurve->Eval(SourceLevel);

	//Effective Armor Calculation
	const FRealCurve* EffectiveArmorCoefficientCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCoefficientCurve->Eval(TargetLevel);
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPen * ArmorPenCoefficient) / 100.f;
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	//Get CriticalHitChance on source and CriticalHitResistance on target and determine if attack was a crit
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Clamp(SourceCriticalHitChance, 0.f, SourceCriticalHitChance);
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Clamp(TargetCriticalHitResistance, 0.f, TargetCriticalHitResistance);

	const FRealCurve* CriticalHitResistanceCoefficientCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCoefficientCurve->Eval(TargetLevel);
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;

	//Check if it was a crit
	if (UAuraAbilitySystemLibrary::RNGRoll(EffectiveCriticalHitChance))
	{
		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
		if (SourceCriticalHitDamage < 0.f) SourceCriticalHitDamage = 0.f;
		Damage += Damage + SourceCriticalHitDamage / 100.f;
		UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
	}
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

float UExecCalc_Damage::CalculateRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle, const float Damage,
                                              const AActor* TargetAvatar)
{
	FVector TargetLocation = TargetAvatar->GetActorLocation();
	const FVector Origin = UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle);

	const float SquareDistance = FVector::DistSquaredXY(TargetLocation, Origin);
	const float SquareInnerRadius = FMath::Square(UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle));
	const float SquareOuterRadius = FMath::Square(UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle));
	if (SquareDistance <= SquareInnerRadius) return Damage;

	const TRange<float> DistanceRange(SquareInnerRadius, SquareOuterRadius);
	const TRange<float> DamageScaleRange(1.f, 0.f);
	const float DamageScale = FMath::GetMappedRangeValueClamped(DistanceRange, DamageScaleRange, SquareDistance);
	return Damage * DamageScale;
}
