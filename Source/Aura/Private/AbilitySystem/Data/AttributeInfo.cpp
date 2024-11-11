// There is no fucking Copyright


#include "AbilitySystem/Data/AttributeInfo.h"

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
		UE_LOG(LogTemp, Error, TEXT("Can not found Attribute Info for Tag '%s'."), *Tag.ToString());
	}
	return {};
}
