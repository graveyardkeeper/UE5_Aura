// There is no fucking Copyright


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "ShaderPrintParameters.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "DataWrappers/ChaosVDQueryDataWrappers.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;
}

/*
 * called on server or standalone.
 */
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();

	LoadProgress();
}

/*
 * called on client.
 */
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->GetPlayerLevel();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	return PS->GetPlayerXP();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	return PS->GetPlayerAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	return PS->GetPlayerSpellPoints();
}

int32 AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	return PS->AddToPlayerXP(InXP);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	PS->AddToPlayerAttributePoints(InPoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	PS->AddToPlayerSpellPoints(InPoints);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpEffect();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* Mat)
{
	AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController());
	check(PC);

	PC->ShowMagicCircle(Mat);
	PC->SetShowMouseCursor(false);
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController());
	check(PC);

	PC->HideMagicCircle();
	PC->SetShowMouseCursor(true);
}


void AAuraCharacter::MulticastLevelUpEffect_Implementation()
{
	ShowLevelUpEffect();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	/* 在这里初始化HUD因为这里才能拿到所需的上面四个变量 */
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			HUD->InitOverlay(PC, PS, AbilitySystemComponent, AttributeSet);
		}
	}

	// 一些广播和回调绑定
	OnAscRegisteredDelegate.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::OnStunTagChanged);
}

void AAuraCharacter::OnRep_Stunned()
{
	// 服务端不需要，当IsStunned复制时，添加/删除标签用来阻止客户端的输入操作
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(AuraTags.Player_Block_CursorTrace);
	BlockedTags.AddTag(AuraTags.Player_Block_InputHeld);
	BlockedTags.AddTag(AuraTags.Player_Block_InputPressed);
	BlockedTags.AddTag(AuraTags.Player_Block_InputReleased);

	if (bIsStunned)
	{
		AuraASC->AddLooseGameplayTags(BlockedTags);
		StunDebuffComponent->Activate();
	}
	else
	{
		AuraASC->RemoveLooseGameplayTags(BlockedTags);
		StunDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	ULoadScreenSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
	if (!SaveData)
	{
		return;
	}
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	SaveData->PlayerStartTag = CheckpointTag;
	SaveData->PlayerLevel = PS->GetPlayerLevel();
	SaveData->PlayerXP = PS->GetPlayerXP();
	SaveData->AttributePoints = PS->GetPlayerAttributePoints();
	SaveData->SpellPoints = PS->GetPlayerSpellPoints();
	SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

	SaveData->bFirstTimeLoadIn = false;
	GameMode->SaveInGameProgressData(SaveData);
}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	ULoadScreenSaveGame* SaveData = GameMode->RetrieveInGameSaveData();
	if (!SaveData)
	{
		return;
	}
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	PS->SetPlayerLevel(SaveData->PlayerLevel);
	PS->SetPlayerXP(SaveData->PlayerXP);
	PS->SetPlayerAttributePoints(SaveData->AttributePoints);
	PS->SetPlayerSpellPoints(SaveData->SpellPoints);

	if (SaveData->bFirstTimeLoadIn)
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
	else
	{
	}

	GameMode->SaveInGameProgressData(SaveData);
}
