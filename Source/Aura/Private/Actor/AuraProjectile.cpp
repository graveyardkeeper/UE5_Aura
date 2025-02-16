// There is no fucking Copyright


#include "Actor/AuraProjectile.h"

#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true); // 不设置的话，客户端看到的位置不对

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (IsValid(LoopingSoundComponent))
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		OnHit();
	}

	Super::Destroyed();
}

bool AAuraProjectile::IsValidOverlap(AActor* OtherActor)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr)
	{
		return false;
	}
	const AActor* SourceActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceActor == OtherActor)
	{
		return false;
	}
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceActor, OtherActor))
	{
		return false;
	}
	return true;
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}
	// 个人理解，生成特效事件和销毁事件都需要server复制到client，但这两个事件到达client的先后顺序不确定
	// 假如生成特效事件先到达，没有任何问题
	// 假如销毁事件先到达，client还未播放特效，此时client不能直接销毁，需要检查bHit（是否已经播放过特效），如果没有，先播放再销毁
	if (!bHit)
	{
		OnHit();
	}

	if (HasAuthority())
	{
		// apply damage effect only on server
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		if (const bool bKnockback = FMath::RandRange(0.f, 100.f) <= DamageEffectParams.KnockbackChance)
		{
			FRotator Rotate = GetActorRotation();
			Rotate.Pitch = 30.f;
			DamageEffectParams.SetKnockbackForce(Rotate.Vector() * DamageEffectParams.KnockbackForceMagnitude);
		}
		DamageEffectParams.SetTargetActor(OtherActor).SetDeathImpulse(DeathImpulse).ApplyDamage();
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
