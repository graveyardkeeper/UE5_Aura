// There is no fucking Copyright


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int32 AAuraPlayerState::AddToPlayerXP(int32 InXP)
{
	return SetPlayerXP(XP + InXP);
}

void AAuraPlayerState::AddToPlayerLevel(int32 InLevel)
{
	// 调用AddTo方法，我们认为是升级场景
	SetPlayerLevel(Level + InLevel, true);
}

void AAuraPlayerState::AddToPlayerAttributePoints(int32 InAttributePoints)
{
	SetPlayerAttributePoints(AttributePoints + InAttributePoints);
}

void AAuraPlayerState::AddToPlayerSpellPoints(int32 InSpellPoints)
{
	SetPlayerSpellPoints(SpellPoints + InSpellPoints);
}


void AAuraPlayerState::SetPlayerLevel(int32 InLevel, bool bLevelUp)
{
	Level = InLevel;
	OnPlayerLevelChangedDelegate.Broadcast(Level, bLevelUp);

	// 等级变更，触发更新技能状态
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->UpdateAbilityStatuses(Level);
}

float AAuraPlayerState::GetPlayerXPPercent() const
{
	if (LevelUpInfo->LevelUpInformation.Num() <= Level + 1)
	{
		// 最后一级的经验百分比永远是100%
		return 1.f;
	}
	float XPStart = LevelUpInfo->LevelUpInformation[Level].LevelRequirementXP;
	float XPEnd = LevelUpInfo->LevelUpInformation[Level + 1].LevelRequirementXP; // 当前不是最后一级，获取右区间
	return (XP - XPStart) / (XPEnd - XPStart);
}

int32 AAuraPlayerState::SetPlayerXP(int32 InXP)
{
	XP = InXP;

	const int32 CurrLevel = Level;
	const int32 NewLevel = LevelUpInfo->FindLevelForXP(XP);

	int32 DeltaLevel = 0;
	if (NewLevel > CurrLevel)
	{
		// Level up
		DeltaLevel = NewLevel - CurrLevel;
		SetPlayerLevel(NewLevel);

		// 计算本次升级一共获得的属性点和技能点
		int32 AttributePointsAward = 0;
		int32 SpellPointsAward = 0;
		for (int32 i = CurrLevel + 1; i <= NewLevel; i++)
		{
			AttributePointsAward += LevelUpInfo->LevelUpInformation[i].AttributePointAward;
			SpellPointsAward += LevelUpInfo->LevelUpInformation[i].SpellPointAward;
		}
		AddToPlayerAttributePoints(AttributePointsAward);
		AddToPlayerSpellPoints(SpellPointsAward);
	}

	OnPlayerXPChangedDelegate.Broadcast(XP);

	return DeltaLevel;
}

void AAuraPlayerState::SetPlayerAttributePoints(int32 InAttributePoints)
{
	if (AttributePoints == InAttributePoints)
	{
		return;
	}
	AttributePoints = InAttributePoints;
	OnPlayerAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetPlayerSpellPoints(int32 InSpellPoints)
{
	if (SpellPoints == InSpellPoints)
	{
		return;
	}
	SpellPoints = InSpellPoints;
	OnPlayerSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldValue)
{
	// TODO: 这里有问题，客户端得知道是不是升级引起的等级变化，先暂时写死true
	OnPlayerLevelChangedDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::OnRep_XP(int32 OldValue)
{
	OnPlayerXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldValue)
{
	OnPlayerAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldValue)
{
	OnPlayerSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
