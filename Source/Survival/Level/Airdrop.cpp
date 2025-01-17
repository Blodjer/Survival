// Copyright 2016 GAMES ACADEMY GmbH

#include "Survival.h"
#include "Airdrop.h"

AAirdrop::AAirdrop()
{
	PayloadCollision = CreateDefaultSubobject<UBoxComponent>("PayloadCollision");
	RootComponent = PayloadCollision;

	BalloonMesh = CreateDefaultSubobject<UStaticMeshComponent>("Balloon");
	BalloonMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	BalloonMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UInterpToMovementComponent>("MovementComponent");
	MovementComponent->AddControlPointPosition(FVector::ZeroVector, true);
	MovementComponent->SetUpdatedComponent(RootComponent);

	Cable1 = CreateDefaultSubobject<UCableComponent>("Cable1");
	Cable2 = CreateDefaultSubobject<UCableComponent>("Cable2");
	Cable3 = CreateDefaultSubobject<UCableComponent>("Cable3");
	Cable4 = CreateDefaultSubobject<UCableComponent>("Cable4");

	CableComponents.Add(Cable1);
	CableComponents.Add(Cable2);
	CableComponents.Add(Cable3);
	CableComponents.Add(Cable4);

	for (int i = 0; i < CableComponents.Num(); i++)
	{
		CableComponents[i]->SetupAttachment(BalloonMesh);

		FVector Direction = FRotator(0.0f, 360.0f / CableComponents.Num() * i, 0.0f).Vector();

		CableComponents[i]->SetRelativeLocation(Direction * 150.0f + FVector(0.0f, 0.0f, -30.0f));
		CableComponents[i]->EndLocation = Direction * PayloadCollision->GetUnscaledBoxExtent().X;

		CableComponents[i]->CableLength = BalloonMesh->RelativeLocation.Z * 0.7f;
		CableComponents[i]->CableWidth = 7.0f;
		CableComponents[i]->NumSides = 6;
	}

	Speed = 500.0f;

	EscapeDistance = 50000.0f;
	EscapeSpeed = 900.0f;

	ReleasePayloadHeight = 350.0f;

	bReplicates = true;
	bReplicateMovement = false;
	bAlwaysRelevant = true;

	PrimaryActorTick.bCanEverTick = true;
}

void AAirdrop::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MovementComponent->OnInterpToStop.AddDynamic(this, &AAirdrop::OnLanded);
}

void AAirdrop::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();

	if (HasAuthority() && Payload == nullptr)
	{
		FVector TestLandingLocation;
		if (MovementComponent->ControlPoints.Num() > 0)
		{
			FVector PositionControlPoint = MovementComponent->ControlPoints.Last().PositionControlPoint;
			TestLandingLocation = MovementComponent->ControlPoints.Last().bPositionIsRelative ? PositionControlPoint + GetActorLocation() : PositionControlPoint;
		}
		else
		{
			TestLandingLocation = FVector::ZeroVector;
		}

		Init(TestLandingLocation, PayloadClass);
	}
}

void AAirdrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && Payload && (GetActorLocation() - LandingLocation).Z <= ReleasePayloadHeight)
	{
		ReleasePayload();
	}
}

void AAirdrop::Init(FVector LandingLocation, TSubclassOf<ADroppablePhysicsActor> PayloadClass)
{
	this->PayloadClass = PayloadClass;

	if (HasAuthority() && PayloadClass != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Instigator = GetInstigator();
		SpawnParameters.Owner = this;

		Payload = GetWorld()->SpawnActor<ADroppablePhysicsActor>(PayloadClass, GetActorLocation(), GetActorRotation(), SpawnParameters);
		if (Payload)
		{
			Payload->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		}
	}

	SetLandingRoute(LandingLocation);
}

void AAirdrop::SetLandingRoute_Implementation(FVector LandingLocation)
{
	this->StartLocation = GetActorLocation();
	this->LandingLocation = LandingLocation;

	MovementComponent->SetUpdatedComponent(RootComponent);

	MovementComponent->Duration = (StartLocation - LandingLocation).Size() / Speed;

	MovementComponent->ControlPoints.Empty();
	MovementComponent->AddControlPointPosition(FVector::ZeroVector, true);
	MovementComponent->AddControlPointPosition(LandingLocation, false);
	MovementComponent->FinaliseControlPoints();

	MovementComponent->RestartMovement();
}

void AAirdrop::OnLanded(const FHitResult& ImpactResult, float Time)
{
	if (!HasAuthority())
		return;

	if (Payload)
	{
		ReleasePayload();
	}
	else
	{
		Destroy(true);
	}
}

void AAirdrop::ReleasePayload()
{
	if (HasAuthority() && Payload)
	{
		Payload->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

		FVector Velocity = (LandingLocation - StartLocation) / MovementComponent->Duration;
		Payload->StartSimulatePhysics(Velocity);

		Payload->SetLifeSpan(120.0f);
		
		Payload = nullptr;

		SetEscapeRoute();
	}
}

void AAirdrop::SetEscapeRoute_Implementation()
{
	MovementComponent->SetUpdatedComponent(RootComponent);

	MovementComponent->Duration = EscapeDistance / EscapeSpeed;

	FVector EscapeLocation = StartLocation - GetActorLocation();
	EscapeLocation = EscapeLocation.RotateAngleAxis(180.0f, FVector(0, 0, 1)).GetSafeNormal();
	EscapeLocation *= EscapeDistance;

	MovementComponent->ControlPoints.Empty();
	MovementComponent->AddControlPointPosition(FVector::ZeroVector, true);
	MovementComponent->AddControlPointPosition(EscapeLocation, true);
	MovementComponent->FinaliseControlPoints();

	MovementComponent->RestartMovement();
}
