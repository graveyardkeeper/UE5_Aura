// There is no fucking Copyright


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName("LoadSlot_0");
	LoadSlot_0->SetSlotIndex(0);
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName("LoadSlot_1");
	LoadSlot_1->SetSlotIndex(1);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName("LoadSlot_2");
	LoadSlot_2->SetSlotIndex(2);
	LoadSlots.Add(2, LoadSlot_2);

	SetNumLoadSlots(LoadSlots.Num());
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
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	LoadSlots[Slot]->SetPlayerName(PlayerName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SetSlotStatus(ESaveSlotStatus::Taken);
	LoadSlots[Slot]->SetMapName(GameMode->DefaultMapName);
	LoadSlots[Slot]->MapAssetName = GameMode->DefaultMap.ToSoftObjectPath().GetAssetName();
	LoadSlots[Slot]->PlayerStartTag = GameMode->DefaultPlayerStartTag;

	GameMode->SaveSlotData(LoadSlots[Slot], Slot);

	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(static_cast<int32>(ESaveSlotStatus::Taken));
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	for (const auto& Pair : LoadSlots)
	{
		bool bEnableButton = true;
		if (Slot == Pair.Key)
		{
			bEnableButton = false;
			SelectedSlot = Pair.Value;
		}
		Pair.Value->EnableSelectSlotButton.Broadcast(bEnableButton);
	}
	SlotSelected.Broadcast();
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (SelectedSlot == nullptr)
	{
		return;
	}
	AAuraGameModeBase::DeleteSlot(SelectedSlot->GetSlotName(), SelectedSlot->GetSlotIndex());
	SelectedSlot->SetSlotStatus(ESaveSlotStatus::Vacant);
	SelectedSlot->InitializeSlot();
	SelectedSlot->EnableSelectSlotButton.Broadcast(true);
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	// 传递玩家出生点
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotName = SelectedSlot->GetSlotName();
	GameInstance->LoadSlotIndex = SelectedSlot->GetSlotIndex();
	GameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;

	GameMode->TravelToMap(SelectedSlot);
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
		LoadSlot->SetPlayerLevel(SaveGameObject->PlayerLevel);
		LoadSlot->SetMapName(SaveGameObject->MapName);
		LoadSlot->PlayerStartTag = SaveGameObject->PlayerStartTag;
		LoadSlot->InitializeSlot();
	}
}
