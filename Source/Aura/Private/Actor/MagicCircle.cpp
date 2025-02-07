// There is no fucking Copyright


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecalComponent");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCircle::SetMagicCircleMaterial(UMaterialInterface* Mat)
{
	MagicCircleDecal->SetDecalMaterial(Mat);
}
