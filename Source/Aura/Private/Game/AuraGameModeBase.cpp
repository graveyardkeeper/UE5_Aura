// There is no fucking Copyright


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetSlotName(), SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->PlayerLevel = LoadSlot->GetPlayerLevel();
	LoadScreenSaveGame->SlotName = LoadSlot->GetSlotName();
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetSlotName(), SlotIndex);
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	return LoadScreenSaveGame;
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	return GetSaveSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveData) const
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	GameInstance->PlayerStartTag = SaveData->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveData, GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World)
{
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	ULoadScreenSaveGame* SaveData = GetSaveSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
	if (!SaveData)
	{
		return;
	}
	if (!SaveData->HasMap(MapName))
	{
		FSavedMap NewSavedMap;
		NewSavedMap.MapAssetName = MapName;
		SaveData->SavedMaps.Add(NewSavedMap);
	}

	TArray<FSavedActor> SavedActors;
	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
		{
			continue;
		}
		FSavedActor SavedActor;
		SavedActor.ActorName = Actor->GetFName();
		SavedActor.Transform = Actor->GetTransform();

		FMemoryWriter Writer(SavedActor.Bytes);
		FObjectAndNameAsStringProxyArchive Archive(Writer, true);
		Archive.ArIsSaveGame = true;
		Actor->Serialize(Archive);

		SavedActors.AddUnique(SavedActor);
	}
	for (FSavedMap& Map : SaveData->SavedMaps)
	{
		if (Map.MapAssetName == MapName)
		{
			Map.SavedActors = SavedActors;
			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(SaveData, GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot) const
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Maps.FindChecked(Slot->GetMapName()));
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Actors);
	if (Actors.Num() == 0)
	{
		return nullptr;
	}
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	AActor* SelectedActor = Actors[0];
	for (AActor* Actor : Actors)
	{
		if (Cast<APlayerStart>(Actor)->PlayerStartTag == GameInstance->PlayerStartTag)
		{
			SelectedActor = Actor;
			break;
		}
	}
	return SelectedActor;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
