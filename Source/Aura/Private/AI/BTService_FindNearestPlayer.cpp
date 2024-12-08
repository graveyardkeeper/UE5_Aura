// There is no fucking Copyright


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();

	FName TargetTag = OwningPawn->ActorHasTag(FName("Enemy")) ? FName("Player") : FName("Enemy");
	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, TargetActors);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	if (TargetActors.Num() == 1)
	{
		ClosestActor = TargetActors[0];
		ClosestDistance = OwningPawn->GetDistanceTo(ClosestActor);
	}
	else
	{
		for (AActor* Actor : TargetActors)
		{
			if (const float Distance = OwningPawn->GetDistanceTo(Actor); Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
