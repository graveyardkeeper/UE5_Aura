// There is no fucking Copyright


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "ShaderPrintParameters.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
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

	// 加载玩家数据
	LoadProgress();
	// 加载世界数据
	Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this))->LoadWorldState(GetWorld());
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

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]
	{
		Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this))->PlayerDied(this);
	});
	GetWorldTimerManager().SetTimer(DeathTimer, TimerDelegate, DeathTime, false);
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
	SaveData->bFirstTimeLoadIn = false;

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

	if (!HasAuthority())
	{
		return;
	}

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	TArray<FSavedAbility> SavedAbilities;
	FForEachAbilityDelegate SaveAbilityDelegate;
	SaveAbilityDelegate.BindLambda([this, AuraASC, &SavedAbilities](const FGameplayAbilitySpec& Spec)
	{
		const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromSpec(Spec);
		if (!AbilityTag.IsValid())
		{
			return;
		}
		FAuraAbilityInfo AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this)->FindAbilityInfoForTag(AbilityTag);

		FSavedAbility SavedAbility;
		SavedAbility.GameplayAbility = AbilityInfo.Ability;
		SavedAbility.AbilityTag = AbilityTag;
		SavedAbility.AbilityInputTag = AuraASC->GetAbilityInputTagFromSpec(Spec);
		SavedAbility.AbilityStatus = AuraASC->GetAbilityStatusFromSpec(Spec);
		SavedAbility.AbilityType = AbilityInfo.AbilityType;
		SavedAbility.AbilityLevel = Spec.Level;

		SavedAbilities.Add(SavedAbility);
	});
	AuraASC->ForEachAbility(SaveAbilityDelegate);
	SaveData->SavedAbilities = SavedAbilities;

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

	if (SaveData->bFirstTimeLoadIn)
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
	else
	{
		// Notice: 先加载技能，再加载等级，先加载等级的话会导致生成一份新的技能Ability，导致问题，可以修复
		Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent())->AddCharacterAbilitiesFromSaveData(SaveData->SavedAbilities);

		AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
		PS->SetPlayerLevel(SaveData->PlayerLevel);
		PS->SetPlayerXP(SaveData->PlayerXP);
		PS->SetPlayerAttributePoints(SaveData->AttributePoints);
		PS->SetPlayerSpellPoints(SaveData->SpellPoints);
		UAuraAbilitySystemLibrary::InitCharacterDefaultAttributesFromSaveData(this, GetAbilitySystemComponent(), SaveData);
	}

	GameMode->SaveInGameProgressData(SaveData);
}
