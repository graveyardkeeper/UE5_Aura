// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
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
	virtual int32 GetCharacterLevel() const override;

protected:
	virtual void InitAbilityActorInfo() override;
};
