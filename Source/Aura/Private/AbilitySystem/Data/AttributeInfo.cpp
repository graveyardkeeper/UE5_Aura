// There is no fucking Copyright


#include "AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Attr : AttributeInformation)
	{
		if (Attr.AttributeTag.MatchesTagExact(Tag))
		{
			return Attr;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can not found Attribute Info for Tag '%s'."), *Tag.ToString());
	}
	return FAuraAttributeInfo();
}
