// There is no fucking Copyright


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AS->GetHealth());
	OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	OnManaChanged.Broadcast(AS->GetMana());
	OnMaxManaChanged.Broadcast(AS->GetMaxMana());
}
