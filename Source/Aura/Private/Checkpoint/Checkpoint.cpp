// There is no fucking Copyright


#include "Checkpoint/Checkpoint.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	// for Highlighting
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilValueOverride);
	CheckpointMesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToLocationComponent = CreateDefaultSubobject<USceneComponent>("MoveToLocation");
	MoveToLocationComponent->SetupAttachment(GetRootComponent());
}

bool ACheckpoint::ShouldLoadTransform_Implementation() const
{
	return false;
}

void ACheckpoint::OnActorLoaded_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToLocationComponent->GetComponentLocation();
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true; // 得先设置，序列化在后

		HandleGlowEffects();

		// 保存玩家数据
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		// 保存世界状态
		Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this))->SaveWorldState(GetWorld());
	}
}

void ACheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, MaterialInstanceDynamic);
	CheckpointReached(MaterialInstanceDynamic);
}
