// There is no fucking Copyright


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.MatchesTagExact(Tag))
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not found AbilityInputAction for InputTag '%s' in class '%s', fill it!"),
		       *Tag.ToString(),
		       *GetNameSafe(this));
	}
	return nullptr;
}
