// Fill out your copyright notice in the Description page of Project Settings.

#include "HideableMesh.h"
#include "HideableMeshActor.h"


// Sets default values
AHideableMeshActor::AHideableMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//static ConstructorHelpers::FObjectFinder<UMaterial> MatObj(TEXT("parentmat"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	SetRootComponent(MeshComp);
}

// Called when the game starts or when spawned
void AHideableMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
#if 0
	if (!MeshComp)
		return;

	int meshId = 0;
	for (UActorComponent* comp : GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(comp);

		// internal counter, prevents shifting if material was not assigned
		int i = 0;
		for (auto& mi : Materials[meshId].MaterialsTranslucent)
		{
			UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(&mi, this);
			//smc->SetMaterial(i, DynMaterial);
			i++;
		}
	}
#endif
	
}


void AHideableMeshActor::OnBecomeOpaque_Implementation()
{
	OnBecomeOpaqueImpl();
}

void AHideableMeshActor::OnBecomeTranslucent_Implementation()
{
	OnBecomeTranslucentImpl();
}

void AHideableMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// don't forget uncomment when strip preprocessor if 0
	//Materials.Empty();

	if (!MeshComp->StaticMesh)
		return;

	PrepareMaterials();
}