// There is no fucking Copyright


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

static AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 重要！需要手动初始化DamageStatics中Tag相关的设置
	DamageStatics().InitializeTagsStuff();

	// 每个伤害型Ability都会有一个Effect，且Effect中会设置一个或多个不同伤害类型的SetByCaller修饰符，其拥有不同伤害类型的Damage.*标签
	// 通过标签，获取此次受到的不同类型伤害值，并累加，即为此次受到的初始总伤害，然后执行每个属性伤害的抵抗计算
	float Damage = 0.f;
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypes2Resistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		const FGameplayEffectAttributeCaptureDefinition ResistanceDef = DamageStatics().Tags2CaptureDefs[
			ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, EvaluateParams, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue = DamageTypeValue * (100.f - Resistance) / 100.f;

		Damage += DamageTypeValue;
	}

	// 捕获目标格档率，计算此次伤害是否被成功格档
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParams,
	                                                           TargetBlockChance);
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);

	// 如果成功格档，降低50%伤害
	const bool bBlocked = TargetBlockChance >= FMath::RandRange(1, 100);
	Damage = bBlocked ? Damage / 2.f : Damage;

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParams, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams,
	                                                           SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);

	// 配置的伤害计算公式的系数
	// this is called only on server, so CharClassInfo must be non-none
	const UCharacterClassInfo* CharClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetCharacterLevel());

	// 计算有效护甲
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(SourceCombatInterface->GetCharacterLevel());

	// 计算最终伤害
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// 计算暴击伤害
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParams,
	                                                           SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(0.f, SourceCriticalHitChance);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParams,
	                                                           SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(0.f, SourceCriticalHitDamage);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParams,
	                                                           TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(0.f, TargetCriticalHitResistance);

	const FRealCurve* CriticalHitResistanceCurve = CharClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(
		TargetCombatInterface->GetCharacterLevel());

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance *
		CriticalHitResistanceCoefficient;
	const bool bCriticalHit = EffectiveCriticalHitChance >= FMath::RandRange(1, 100);

	// 暴击最终伤害=原伤害*2 + 暴击额外伤害
	Damage = bCriticalHit ? Damage * 2.f + SourceCriticalHitDamage : Damage;

	// 设置伤害标记
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// 通过设置目标上的IncomingDamage属性来造成伤害，而不是直接设置Health
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                                   EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
