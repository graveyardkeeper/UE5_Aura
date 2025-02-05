// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveSpellNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveSpellNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UPassiveSpellNiagaraComponent();

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

	virtual void BeginPlay() override;

	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
};
