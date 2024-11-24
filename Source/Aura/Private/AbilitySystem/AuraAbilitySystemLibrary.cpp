// There is no fucking Copyright


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			const FWidgetControllerParams Params(PC, PS, PS->GetAbilitySystemComponent(), PS->GetAttributeSet());
			return HUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			const FWidgetControllerParams Params(PC, PS, PS->GetAbilitySystemComponent(), PS->GetAttributeSet());
			return HUD->GetAttributeMenuWidgetController(Params);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitCharacterDefaultAttributes(const UObject* WorldContextObject,
                                                               UAbilitySystemComponent* ASC,
                                                               ECharacterClass CharacterClass,
                                                               float Level)
{
	const AAuraGameModeBase* GameMode = CastChecked<AAuraGameModeBase>(
		UGameplayStatics::GetGameMode(WorldContextObject));

	const UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;
	const FCharacterClassDefaultInfo ClassDefaults = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttrCtx = ASC->MakeEffectContext();
	PrimaryAttrCtx.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle PrimaryAttrSpecHandle = ASC->MakeOutgoingSpec(
		ClassDefaults.PrimaryAttributes,
		Level,
		PrimaryAttrCtx);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttrSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttrCtx = ASC->MakeEffectContext();
	SecondaryAttrCtx.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SecondaryAttrSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttributes,
		Level,
		SecondaryAttrCtx);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttrSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttrCtx = ASC->MakeEffectContext();
	VitalAttrCtx.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle VitalAttrSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttributes,
		Level,
		VitalAttrCtx);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttrSpecHandle.Data.Get());
}
