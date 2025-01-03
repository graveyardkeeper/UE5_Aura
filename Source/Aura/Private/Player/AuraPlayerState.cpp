// There is no fucking Copyright


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
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
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddToPlayerXP(int32 InXP)
{
	SetPlayerXP(XP + InXP);
}

void AAuraPlayerState::AddToPlayerLevel(int32 InLevel)
{
	SetPlayerLevel(Level + InLevel);
}

void AAuraPlayerState::SetPlayerLevel(int32 InLevel)
{
	Level = InLevel;
	OnPlayerLevelChangedDelegate.Broadcast(Level);
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

void AAuraPlayerState::SetPlayerXP(int32 InXP)
{
	XP = InXP;
	Level = LevelUpInfo->FindLevelForXP(XP);
	OnPlayerXPChangedDelegate.Broadcast(XP);
}


void AAuraPlayerState::OnRep_Level(int32 OldValue)
{
	OnPlayerLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldValue)
{
	OnPlayerXPChangedDelegate.Broadcast(XP);
}
