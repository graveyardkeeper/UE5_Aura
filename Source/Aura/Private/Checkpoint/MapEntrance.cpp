// There is no fucking Copyright


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToLocationComponent);
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::OnActorLoaded_Implementation()
{
	// Do nothing.
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true; // 得先设置，序列化在后

		HandleGlowEffects();

		// 保存玩家数据
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		// 保存当前世界，但把地图名称设置为目标地图
		Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this))->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
