// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipController.h"
#include "EnemyController.h"
#include "SpaceShooterGameMode.h"

// Sets default values
AShipController::AShipController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a dummy root component we can attach things to.
	// RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AShipController::OnOverlap);

}

// Called when the game starts or when spawned
void AShipController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShipController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurLocation = GetActorLocation();

	if (!CurrentVelocity.IsZero()) {
		FVector NewLocation = CurLocation +
			Speed * CurrentVelocity * DeltaTime;
		SetActorLocation(NewLocation);
		UE_LOG(LogTemp, Log, TEXT("CurVelocity: %s CurLocation: %s NewLocation: %s"), *CurrentVelocity.ToString(), *CurLocation.ToString(), *NewLocation.ToString());
	}

}

// Called to bind functionality to input
void AShipController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &AShipController::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AShipController::Move_YAxis);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShipController::OnShoot);
	InputComponent->BindAction("Restart", IE_Pressed, this, &AShipController::OnRestart).bExecuteWhenPaused = true;
}

void AShipController::Move_XAxis(float AxisValue) {
	CurrentVelocity.X = AxisValue * 100.0f;
	if (AxisValue != 0.0f)
		UE_LOG(LogTemp, Log, TEXT("Move_XAxis %f new CurrVelocity %s"), AxisValue, *CurrentVelocity.ToString());
}

void AShipController::Move_YAxis(float AxisValue) {
	CurrentVelocity.Y = AxisValue * 100.0f;
	if (AxisValue != 0.0f)
		UE_LOG(LogTemp, Log, TEXT("Move_YAxis %f new CurrVelocity %s"), AxisValue, *CurrentVelocity.ToString());
}

void AShipController::OnShoot() {

	UE_LOG(LogTemp, Log, TEXT("shoot!"));
	UWorld* World = GetWorld();

	if (World)
	{
		FVector Location = GetActorLocation();

		World->SpawnActor<ABulletController>(BulletBlueprint, Location, FRotator::ZeroRotator);

	}
}

void AShipController::OnOverlap(UPrimitiveComponent *OverlappedComponent,
				  AActor* OtherActor,
				  UPrimitiveComponent* OtherComponent,
				  int32 OtherBodyIndex,
				  bool bFromSweep,
				  const FHitResult& SweepResult) {
    if (OtherActor->IsA(AEnemyController::StaticClass())) {
		Died = true;

		this->SetActorHiddenInGame(true);

		((ASpaceShooterGameMode *)GetWorld()->GetAuthGameMode())->OnGameOver();
		
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void AShipController::OnRestart()
{
	if (Died) {
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}