// There is no fucking Copyright


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
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

void AAuraGameModeBase::SaveWorldState(UWorld* World) const
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

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!UGameplayStatics::DoesSaveGameExist(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex))
	{
		return;
	}

	ULoadScreenSaveGame* SaveData = GetSaveSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
	if (!SaveData || !SaveData->HasMap(MapName))
	{
		UE_LOG(LogAura, Error, TEXT("Failed to load slot, or the map does not exist in the saved data!"));
		return;
	}
	FSavedMap SavedMap = SaveData->GetSavedMapWithMapName(MapName);
	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
		{
			continue;
		}
		for (const FSavedActor& SavedActor : SavedMap.SavedActors)
		{
			// TODO: 这里性能有点拉跨，可以优化下
			if (Actor->GetFName() != SavedActor.ActorName)
			{
				continue;
			}
			if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
			{
				Actor->SetActorTransform(SavedActor.Transform);
			}
			FMemoryReader Reader(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(Reader, true);
			Archive.ArIsSaveGame = true;

			Actor->Serialize(Archive); // 序列化和反序列化都是Serialize方法

			// Actor完成加载，通知
			ISaveInterface::Execute_OnActorLoaded(Actor);
		}
	}
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
