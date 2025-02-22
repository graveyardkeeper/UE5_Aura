// There is no fucking Copyright


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName("LoadSlot_0");
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName("LoadSlot_1");
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName("LoadSlot_2");
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("New Game: %d"), Slot));
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FText& SlotName)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("New Slot: %d, SlotName: %s"), Slot, *SlotName.ToString()));
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Select Slot: %d"), Slot));
}
