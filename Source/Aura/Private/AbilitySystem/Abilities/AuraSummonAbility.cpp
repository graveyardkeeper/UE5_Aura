// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include "Aura/Aura.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	// 从左到右顺时针旋转，初始往逆时针方向旋转SpawnSpread一半的角度，之后每次旋转DeltaAngle个角度
	FVector StartOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	float DeltaAngle = 0.f;
	if (NumMinions > 1)
	{
		DeltaAngle = SpawnSpread / (NumMinions - 1); // 这里，需要-1，因为是生成在线上，n条线只用旋转n-1次
	}
	else
	{
		StartOfSpread = Forward; // 只有一条线，使用Forward，for中的旋转无效
	}

	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = StartOfSpread.RotateAngleAxis(i * DeltaAngle, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		// 如果在斜面上，找到贴地的位置
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation + FVector::UpVector * 500,
		                                     SpawnLocation - FVector::UpVector * 500, ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			SpawnLocation = HitResult.ImpactPoint;
		}
		SpawnLocations.Add(SpawnLocation);
#ifdef AURA_DEBUG
		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Location, Location + Direction * MaxSpawnDistance, 4.f,
		                                     FLinearColor::Red, 2.f);
		DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::White, false, 2.f);
		DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::White, false, 2.f);
		DrawDebugSphere(GetWorld(), SpawnLocation, 10.f, 12, FColor::Green, false, 2.f);
#endif
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	check(!MinionClasses.IsEmpty());
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
