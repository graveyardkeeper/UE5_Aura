// There is no fucking Copyright


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadSlot::SetLoadSlotName(const FString& InSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InSlotName);
}
