// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "PickupSpawnerComponent.h"

UPickupSpawnerComponent::UPickupSpawnerComponent()
{
	bWantsBeginPlay = true;
	bAutoActivate = true;
	bUseAttachParentBound = true;
	
#if WITH_EDITORONLY_DATA
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
	bVisualizeComponent = true;
#else
	PrimaryComponentTick.bCanEverTick = false;
	bTickInEditor = false;
#endif
}

#if WITH_EDITORONLY_DATA
void UPickupSpawnerComponent::OnRegister()
{
	Super::OnRegister();

	UpdateVisualizeSprite();
}
#endif

#if WITH_EDITOR
void UPickupSpawnerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdateVisualizeSprite();
}
#endif

void UPickupSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	Spawn();
}

#if WITH_EDITOR
void UPickupSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TickType == ELevelTick::LEVELTICK_ViewportsOnly)
	{
		UpdateVisualizeBox();
	}
}

void UPickupSpawnerComponent::UpdateVisualizeSprite()
{
	if (SpriteComponent)
	{
		SpriteComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));

		SpriteComponent->SpriteInfo.Category = TEXT("Misc");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT("MiscCategory", "Misc", "Misc");
		
		SpriteComponent->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/Spawn_Point.Spawn_Point")));
	}
}

void UPickupSpawnerComponent::UpdateVisualizeBox()
{
	if (GetWorld() == nullptr || GetWorld()->bBegunPlay || GetWorld()->ViewLocationsRenderedLastFrame.Num() == 0)
		return;
	
	if (FVector::DistSquared(GetComponentLocation(), GetWorld()->ViewLocationsRenderedLastFrame[0]) < 8500000)
	{
		FVector MaxExtend = FVector::ZeroVector;
		for (FPickupSpawnerItem& Item : SpawnList)
		{
			if (Item.Pickup && Item.Pickup.GetDefaultObject()->GetPickupMesh())
			{
				FVector Extend = Item.Pickup.GetDefaultObject()->GetPickupMesh()->CalcBounds(FTransform(GetComponentLocation())).BoxExtent;
				MaxExtend.X = FMath::Max(MaxExtend.X, Extend.X);
				MaxExtend.Y = FMath::Max(MaxExtend.Y, Extend.Y);
				MaxExtend.Z = FMath::Max(MaxExtend.Z, Extend.Z);
			}
		}

		FVector Location = GetComponentLocation() + GetComponentRotation().RotateVector(FVector(0, 0, MaxExtend.Z));
		DrawDebugSolidBox(GetWorld(), Location, MaxExtend, GetComponentRotation().Quaternion(), FColor(255, 0, 255, 10), false);
	}
}
#endif

void UPickupSpawnerComponent::Spawn()
{
	if (!GetWorld()->IsServer())
		return;

	if (SpawnList.Num() <= 0)
		return;

	TSubclassOf<APickup> SpawnPickupClass;
	if (SpawnList.Num() == 1)
	{
		SpawnPickupClass = SpawnList[0].Pickup;
	}
	else
	{
		float TotalProbability = 0.0f;
		for (FPickupSpawnerItem Item : SpawnList)
		{
			TotalProbability += Item.Probability;
		}

		float ItemProbabilityIndex = FMath::RandRange(0.0f, TotalProbability);
		float i = 0.0f;
		for (FPickupSpawnerItem Item : SpawnList)
		{
			i += Item.Probability;
			if (i >= ItemProbabilityIndex)
			{
				SpawnPickupClass = Item.Pickup;
				break;
			}
		}
	}

	if (SpawnPickupClass != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = GetAttachmentRootActor();

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(SpawnPickupClass, GetComponentLocation(), GetComponentRotation(), SpawnParameters);
		if (Pickup)
		{
			if (GetAttachParent() != nullptr && GetAttachParent()->GetAttachmentRootActor()->bNetLoadOnClient)
			{
				Pickup->AttachToComponent(GetAttachParent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			}
		}
	}
}
