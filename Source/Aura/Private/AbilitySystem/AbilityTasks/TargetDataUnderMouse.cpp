// There is no fucking Copyright


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
}

void UTargetDataUnderMouse::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		// 当前在客户端，向服务器发送鼠标位置数据
		SendMouseCursorData();
	}
	else
	{
		// 当前在服务端，注册委托，当数据到达服务器时回调
		AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).
		                        AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// called on client.
	FScopedPredictionWindow ScopedPrediction = FScopedPredictionWindow(AbilitySystemComponent.Get());

	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// send to server
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(),
	                                                      GetActivationPredictionKey(),
	                                                      DataHandle,
	                                                      FGameplayTag(),
	                                                      AbilitySystemComponent->ScopedPredictionKey);
	// 固定套路，当前ability是否还有效
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
                                                           FGameplayTag ActivationTag)
{
	// called on server, target data is arrived.

	// 消耗target data，意味着数据成功到达，让ASC不再关心target data的事宜
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
