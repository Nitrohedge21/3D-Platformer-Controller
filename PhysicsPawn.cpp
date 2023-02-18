// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsPawn.h"
#include <Kismet/KismetMathLibrary.h>


//NOTES TO MYSELF
// IMPORTANT: Check either tick or checkBoost to see how the transition was made smoothly rather than instantly with the use of Lerp.
// 
// Tried changing the scale of sphere to see if the camera issue was being caused by the pawn's mesh. Turns out it wasn't the case.
// Instead of moving the camera, I might be able to change the fov to give the same feeling as if the camera moved.
// OR, Changing the target arm length could also work????????????
//Take a look at how certain physics work in games like unleashed, 06 and generations.
//Does the camera rotate the player? Is the player able to rotate to the direction of the camera?(this is probably how it was but it should not rotate the player with the camera at the same time.)
//IMPORTANT: Check boosting function to see how to make an action repeat!
//IMPORTANT: Make it so that the force that is being applied doesn't stack up after a certain amount.
//Might need to change the movement code yet again because I need to make it so that it's unable to speed up after a certain point but I don't know how since the current way is applying force rather than speed.

APhysicsPawn::APhysicsPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	speed = APhysicsPawn::GetVelocity().X;//This might be problematic, gotta check it out with someone.
	boostSpeed = 5000.0f;
	boostForce = 10.0f;
	displayTime = 99999999;
	isJumping = false;
	isBoosting = false;
	pushForce = 10.0f;
	currentStamina = 1.0f;
	maxStamina = 1.0f;
	staminaSprintUsageRate = 0.1f;
	staminaRechargeRate = 0.1f;

	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	PawnMesh->SetWorldScale3D(FVector(1.5, 1.5, 1.5));
	PawnMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -75.0f));
	PawnMesh->SetStaticMesh(MeshRef.Object);
	PawnMesh->SetupAttachment(GetRootComponent());
	PawnMesh->SetSimulatePhysics(false);
	PawnMesh->BodyInstance.bLockXRotation = true;
	PawnMesh->BodyInstance.bLockYRotation = true;
	PawnMesh->BodyInstance.bLockZRotation = true;
	
	MovementComponent->bOrientRotationToMovement = true;
	MovementComponent->RotationRate = FRotator(0.0f, 2000.0f, 0.0f);
	// This used to be 500 but it was not turning to the faced direction in an instant. The documentation doesn't say shit either. Gotta Ask Matt
	MovementComponent->MaxWalkSpeed = 2500.0f;
	MovementComponent->JumpZVelocity = 400;
	MovementComponent->GravityScale = 1.5;
	JumpMaxHoldTime = 0.5f;
	MovementComponent->GroundFriction = 20;
	MovementComponent->TouchForceFactor = speed / 0.5;
	MovementComponent->BrakingFrictionFactor = 0;
	MovementComponent->MaxAcceleration = 1536;
	//MovementComponent->bScalePushForceToVelocity;
	//Check the definition of this function.

	//MovementComponent->PushForceFactor = APhysicsPawn::GetVelocity();
	
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = true;
	CapsuleComponent->SetCapsuleHalfHeight(75);
	CapsuleComponent->SetCapsuleRadius(40);
	CapsuleComponent->SetRenderCustomDepth(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 625.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	FollowCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	FollowCamera->FieldOfView = 90.0f;
}

void APhysicsPawn::Move_FB(float axis)
{
	if (axis != 0.0f)
	{
		if (CapsuleComponent)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, axis);
		}
	}
}

void APhysicsPawn::Move_LR(float axis)
{

	if (axis != 0.0f)
	{
		if (CapsuleComponent)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, axis);
			
		}
	}
}

void APhysicsPawn::boostStart()
{
	//boost start and end needs more work on them, ask Matt for help.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Boosting"));
	isBoosting = true;
}

void APhysicsPawn::boostEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Running or walking"));
	isBoosting = false;
}

void APhysicsPawn::BoostLaunch(float axis)
{
	//This function is going to stay here till i get the mechanic checked.
	if (axis != 0.0f)
	{
		if (CapsuleComponent)
		{
			const FVector Forward = GetActorRotation().Vector();
			AddMovementInput(Forward, axis);
		}
	}
}

// [EXPERIMENTAL FUNCTION!!!!]
void APhysicsPawn::raycastLine()
{
	float lineLength = 40;
	FHitResult OutHit;
	FVector Start = CapsuleComponent->GetComponentLocation();
	FVector ForwardVector = CapsuleComponent->GetForwardVector();
	Start = Start + (ForwardVector * lineLength); // Set the starting point of the line
	FVector End = Start + (ForwardVector * 150.f);	//Set the end point of the line

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());
	
	//Draws the raycast line
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 1);
	bool IsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	if (IsHit)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Blue, TEXT("Raycast Line Collided with something"));
	}
}

void APhysicsPawn::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void APhysicsPawn::Tick(float DeltaTime)
{
	//This part is essentially Update() from Unity. Super::Tick(DeltaTime); seems like it might be FixedUpdate() but gotta research this a bit further to see whether if it is useful or not.
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(0, displayTime, FColor::Green, FString::Printf(TEXT("SPEED: %s"), *APhysicsPawn::GetVelocity().ToString()));
	GEngine->AddOnScreenDebugMessage(0, displayTime, FColor::Blue, FString::Printf(TEXT("STAMINA: %f"), APhysicsPawn::currentStamina));
	//GEngine->AddOnScreenDebugMessage(0, displayTime, FColor::Green, FString::Printf(TEXT("Current Jump Count: %d"), APhysicsPawn::JumpCurrentCount));

	raycastLine();

	if (isBoosting == true && currentStamina > 0)
	{
		//[TODO] Make the camera rotate to the direction that sonic is facing.
		//These lines don't work outside of the Tick for some reason.
		for (int i = 0; i < 1; i++)
		{
			BoostLaunch(1.0f);	//The float value has to be set to 1.0f in order to actually make Sonic move.
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Initial Boost Launch"));
			FollowCamera->bUsePawnControlRotation = true;
		}

		MovementComponent->MaxWalkSpeed = FMath::Lerp(MovementComponent->MaxWalkSpeed, boostSpeed, 0.1f);
		//Make the player able to jump while in air. - Genuinely have no clue what I was thinking about here, might be a typo. 02/02/2023
		FollowCamera->SetFieldOfView(FMath::Lerp(FollowCamera->FieldOfView, 80.0f, 0.1f));

		//Uses stamina while boost is happening
		currentStamina = FMath::FInterpConstantTo(currentStamina, 0.0f, DeltaTime, staminaSprintUsageRate);
	}
	else if(isBoosting == false || (isBoosting == false && currentStamina < maxStamina))
	{
		MovementComponent->MaxWalkSpeed = FMath::Lerp(MovementComponent->MaxWalkSpeed, 2500.0f, 0.1f);
		//Recharges stamina if it's lower than max stamina
		currentStamina = FMath::FInterpConstantTo(currentStamina, maxStamina, DeltaTime, staminaRechargeRate);
		camReset();
		FollowCamera->bUsePawnControlRotation = false;
	}
	if (currentStamina == 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Ran out of stamina!"));
	}
}

void APhysicsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Move_FB"), this, &APhysicsPawn::Move_FB);
	PlayerInputComponent->BindAxis(TEXT("Move_LR"), this, &APhysicsPawn::Move_LR);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APhysicsPawn::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APhysicsPawn::StopJumping);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &APhysicsPawn::boostStart);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &APhysicsPawn::boostEnd);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &APhysicsPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down", this, &APhysicsPawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Stomp", IE_Pressed, this, &APhysicsPawn::Stomp);
	PlayerInputComponent->BindAction("Jump Dash", IE_Pressed, this, &APhysicsPawn::JumpDash);

}

void APhysicsPawn::Stomp()
{
	//[IMPORTANT] Check Sonic Unleashed to see if you can stomp without needing to jump first.
	// This fixed the spamming issue but might not be a good idea.
	if (MovementComponent->IsFalling() && JumpCurrentCount == 1)
	{
		stompForce = 2500;
		const FVector Downward = -GetActorUpVector();
		// Made it override XY so that it does not maintain the momentum
		LaunchCharacter(Downward * stompForce, false, true);
		JumpCurrentCount = 2; // StompJumping(); might be more useful for this part.
	}
}

void APhysicsPawn::JumpDash()
{
	//I can use the code inside to make it do the dash but gotta fix the if statement.
	if (!MovementComponent->IsMovingOnGround()&& JumpCurrentCount == 1)
	{
		jumpDashForce = 900;
		const FVector Forward = GetActorRotation().Vector();
		LaunchCharacter(Forward * jumpDashForce, false, false);
		JumpCurrentCount = 2;
	}
}

void APhysicsPawn::camReset()
{
	//Sets the fov back to the default one if sonic isn't boosting.
	//[IMPORTANT] Make it so that it's set to 100 for a few seconds and then goes back to 80. Default value should be 90. 
	if (!isBoosting)
	{
		FollowCamera->SetFieldOfView(FMath::Lerp(FollowCamera->FieldOfView, 100.0f, 0.1f));
	}
}

void APhysicsPawn::checkBoostMeter()
{
	//Make a stamina bar that disables the boost ability when it's empty.
	//Take a look at the sprinting guide to perhaps be able to fix this.
}

