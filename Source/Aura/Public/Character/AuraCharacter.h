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

	/** Player Interface. */
	virtual int32 GetXP_Implementation() const override;
	virtual int32 AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;

protected:
	virtual void InitAbilityActorInfo() override;
};
