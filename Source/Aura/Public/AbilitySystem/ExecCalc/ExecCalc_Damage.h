// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

/**
 *	伤害相关的静态定义
 */
struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> Tags2CaptureDefs;

	bool bHasInitialedTagsStuff = false; // 是否已经初始化过Tag相关的事情

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}

	/**
	 * 注意要引用Tags，就不能在构造函数中进行，因为AuraGameplayTags还没有完成初始化
	 */
	void InitializeTagsStuff()
	{
		if (bHasInitialedTagsStuff)
		{
			return;
		}
		bHasInitialedTagsStuff = true;

		// Map tag to defs
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_Armor, ArmorDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_ArmorPenetration, ArmorPenetrationDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_BlockChance, BlockChanceDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitChance, CriticalHitChanceDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

		Tags2CaptureDefs.Add(Tags.Attribute_Resistance_Fire, FireResistanceDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Resistance_Lightning, LightningResistanceDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Resistance_Arcane, ArcaneResistanceDef);
		Tags2CaptureDefs.Add(Tags.Attribute_Resistance_Physical, PhysicalResistanceDef);
	}
};

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
