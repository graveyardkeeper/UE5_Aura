// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UPassiveSpellNiagaraComponent;
class UDebuffNiagaraComponent;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;


UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	/** 类中有Replicated的变量时，必须重写这个 */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	void SetCharacterClass(ECharacterClass InClass) { CharacterClass = InClass; }

	/** Combat Interface */
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncreaseMinionCount_Implementation(int32 Amount) override;
	virtual FOnAscRegisteredDelegate& GetOnAscRegisteredDelegate() override;
	virtual FOnDeathDelegate& GetOnDeathDelegate() override;
	virtual FOnDamageDelegate& GetOnDamageDelegate() override;
	virtual void SetIsBeingShocked_Implementation(bool bInIsBeingShocked) override;
	virtual bool GetIsBeingShocked_Implementation() override;

	/** 多播，角色死亡时在所有客户端上的表现 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void InitAbilityActorInfo();

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const;

	virtual void InitializeDefaultAttributes() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName TailSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	FOnAscRegisteredDelegate OnAscRegisteredDelegate;
	FOnDeathDelegate OnDeathDelegate;
	FOnDamageDelegate OnDamageDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes; // effect to initial vital attributes.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes; // effect to initial primary attributes.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes; // effect to initial secondary attributes.

	void AddCharacterAbilities();

	/** 死亡后溶解的材质实例 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	/** 执行溶解 */
	void Dissolve();

	/** Timeline动态设置材质参数实现溶解动画，角色和武器不能共用同一个Timeline*/
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 600.f;

	/** 是否眩晕 */
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly, Category="Combat")
	bool bIsStunned = false;

	/** 是否灼烧 */
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly, Category="Combat")
	bool bIsBurned = false;

	/** 是否正在被电击 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Combat")
	bool bIsBeingShocked = false;

	UFUNCTION()
	virtual void OnRep_Stunned();

	UFUNCTION()
	virtual void OnRep_Burned();

	virtual void OnStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	/** Minions */
	int32 MinionCount = 0;

	/** Debuff 特效 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;

	/** 被动技能特效 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveSpellNiagaraComponent> HaloOfProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveSpellNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveSpellNiagaraComponent> ManaSiphonNiagaraComponent;

private:
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TArray<FTaggedMontage> AttackMontages;
};
