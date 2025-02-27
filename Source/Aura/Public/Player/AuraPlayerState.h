// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

// 玩家数据变更委托，如经验、等级等不在AttributeSet中的属性
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedDelegate, int32 /*New Value*/);
// 等级变更单独一个，需要加个参数代表是否是正常升级还是被动设置升级（如载入进度），影响是否展示升级动画
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerLevelChangedDelegate, int32 /*New Value*/, bool /* bLevelUp */);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FOnPlayerStatChangedDelegate OnPlayerXPChangedDelegate;
	FOnPlayerLevelChangedDelegate OnPlayerLevelChangedDelegate;
	FOnPlayerStatChangedDelegate OnPlayerAttributePointsChangedDelegate;
	FOnPlayerStatChangedDelegate OnPlayerSpellPointsChangedDelegate;

	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetPlayerXP() const { return XP; }
	FORCEINLINE int32 GetPlayerAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetPlayerSpellPoints() const { return SpellPoints; }

	int32 AddToPlayerXP(int32 InXP); // 返回升级的级数
	void AddToPlayerLevel(int32 InLevel);
	void AddToPlayerAttributePoints(int32 InAttributePoints);
	void AddToPlayerSpellPoints(int32 InSpellPoints);

	int32 SetPlayerXP(int32 InXP);
	void SetPlayerLevel(int32 InLevel, bool bLevelUp = false);
	void SetPlayerAttributePoints(int32 InAttributePoints);
	void SetPlayerSpellPoints(int32 InSpellPoints);

	float GetPlayerXPPercent() const;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category="LevelUpInformation")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldValue);

	UFUNCTION()
	void OnRep_XP(int32 OldValue);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldValue);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldValue);
};
