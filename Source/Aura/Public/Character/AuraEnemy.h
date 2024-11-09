// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	/*
	 * Enemy interface.
	 */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	/*
	 * Combat interface.
	 */
	virtual int32 GetCharacterLevel() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CharacterClassDefault")
	int32 Level = 1;

	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;
};
