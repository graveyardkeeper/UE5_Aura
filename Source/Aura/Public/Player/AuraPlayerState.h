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
	FOnPlayerStatChangedDelegate OnPlayerLevelChangedDelegate;

	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetPlayerXP() const { return XP; }

	void AddToPlayerXP(int32 InXP);
	void AddToPlayerLevel(int32 InLevel);

	void SetPlayerXP(int32 InXP);
	void SetPlayerLevel(int32 InLevel);

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

	UFUNCTION()
	void OnRep_Level(int32 OldValue);

	UFUNCTION()
	void OnRep_XP(int32 OldValue);
};
