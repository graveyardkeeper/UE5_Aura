// There is no fucking Copyright


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName("LoadSlot_0");
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName("LoadSlot_1");
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName("LoadSlot_2");
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(static_cast<int32>(ESaveSlotStatus::EnterName));
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FText& PlayerName)
{
	LoadSlots[Slot]->SetPlayerName(PlayerName);
	LoadSlots[Slot]->SetSlotStatus(ESaveSlotStatus::Taken);
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	GameMode->SaveSlotData(LoadSlots[Slot], Slot);

	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(static_cast<int32>(ESaveSlotStatus::Taken));
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Select Slot: %d"), Slot));
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const auto& Pair : LoadSlots)
	{
		UMVVM_LoadSlot* LoadSlot = Pair.Value;
		ULoadScreenSaveGame* SaveGameObject = GameMode->GetSaveSlotData(LoadSlot->GetSlotName(), Pair.Key);
		LoadSlot->SetSlotStatus(SaveGameObject->SaveSlotStatus);
		LoadSlot->SetPlayerName(SaveGameObject->PlayerName);
		LoadSlot->InitializeSlot();
	}
}
