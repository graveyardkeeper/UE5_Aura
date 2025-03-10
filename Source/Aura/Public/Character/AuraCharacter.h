// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	/** Combat Interface. */
	/**
	 * GetCharacterLevel不在父类实现，而是分别在AuraCharacter和Enemy中实现，因为玩家的Level不维护在Character中（Avatar），而是在PlayerState中
	 */
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual void Die(const FVector& DeathImpulse) override;

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	/** Player Interface. */
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual int32 AddToXP_Implementation(int32 InXP) override;
	virtual void AddToAttributePoints_Implementation(int32 InPoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InPoints) override;
	virtual void LevelUp_Implementation() override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* Mat) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;

protected:
	virtual void InitAbilityActorInfo() override;

	/**这里略显复杂了，因为特效在蓝图中实现，如果只想Server或单机显示特效，直接可以在蓝图中实现PlayerInterface中的LevelUp接口
	 * 如果想广播到客户端，必须有Multicast注解，但BlueprintImplementableEvent和NetMulticast不能同时存在，所以拆成了两步
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void ShowLevelUpEffect();

	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;

	void LoadProgress();
};
