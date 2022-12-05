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

	speed = 1000.0f;
	boostSpeed = 2500.0f;
	displayTime = 99999999;
	//jumpForce = 500.0f;
	isJumping = false;
	isBoosting = false;
	

	pushForce = 10.0f;
	currentStamina = 1.0f;
	maxStamina = 1.0f;
	staminaSprintUsageRate = 0.05f;
	staminaRechargeRate = 0.1f;

	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));


	PawnMesh->SetStaticMesh(MeshRef.Object);
	PawnMesh->SetupAttachment(GetRootComponent());
	PawnMesh->SetSimulatePhysics(false);
	PawnMesh->BodyInstance.bLockXRotation = true;
	PawnMesh->BodyInstance.bLockYRotation = true;
	PawnMesh->BodyInstance.bLockZRotation = true;
	//PawnMesh->SetRelativeScale3D(FVector(1.25,1.25,1.25));
	//Uncomment it if you want to go back to capsule
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	MovementComponent->MaxWalkSpeed = 1000.0f;
	MovementComponent->JumpZVelocity = 300;
	JumpMaxHoldTime = 0.5f;
	
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	//CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	//[Important] Gotta fix on the camera's location and rotation!!!!!!!!!

	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetRelativeLocation(FVector(-225.0f, 0.0f, 105.0f));
	FollowCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	FollowCamera->FieldOfView = 90.0f;

	//Sets the size and location of things, got the reference from the blueprint.
	PawnMesh->SetRelativeLocation(FVector(0, 0, -50));
	//Apparently I made the line above to put the mesh inside the collider but i think this isn't the right way of doing it.
	CapsuleComponent->SetCapsuleHalfHeight(52);
	CapsuleComponent->SetCapsuleRadius(52);
	//Set these back to 61x32 if you decide to set the object to sphere again. and the relative location to -60
	//There might be an easier way of setting the capsule's size and height, saw something similar in the template's controller.
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
	//MovementComponent->MaxWalkSpeed = boostSpeed;
	if (speed >= boostSpeed)
	{
		speed = boostSpeed;
	}
}

void APhysicsPawn::boostEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Running or walking"));
	isBoosting = false;
	//MovementComponent->MaxWalkSpeed = speed;
	
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

	GEngine->AddOnScreenDebugMessage(0, displayTime, FColor::Green, FString::Printf(TEXT("SPEED: %s"), *APhysicsPawn::GetVelocity().ToString()));
	//GEngine->AddOnScreenDebugMessage(0, displayTime, FColor::Blue, FString::Printf(TEXT("STAMINA: %f"), APhysicsPawn::currentStamina));
	if (isBoosting == true)
	{
		//LaunchCharacter seems useful when it's mid air but acts weird when it's on ground.
		int launchForce = 110;
		const FVector Forward = this->GetActorRotation().Vector();
		LaunchCharacter(Forward * launchForce, false, false);
		//Make the player able to jump while in air.
		FollowCamera->SetFieldOfView(FMath::Lerp(FollowCamera->FieldOfView, 80.0f, 0.1f));

		//Uses stamina while boost is happening
		currentStamina = FMath::FInterpConstantTo(currentStamina, 0.0f, DeltaTime, staminaSprintUsageRate);
	}
	else
	{
		camReset();
		if (currentStamina < maxStamina)
		{
			//Recharges stamina if it's lower than max stamina
			currentStamina = FMath::FInterpConstantTo(currentStamina, maxStamina, DeltaTime, staminaRechargeRate);
		}
		
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

}

void APhysicsPawn::Stomp()
{
	//Tried using bPressedJump to check if it's in air but didn't work.
	//I don't think that it's gonna be an issue doing it this way.
	int launchForce = 2000;
	const FVector Downward = -GetActorUpVector();
	LaunchCharacter(Downward * launchForce, false, true);
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
}

//Removing this code solved the issue with jump not working properly.
//but changing the !V.IsNearlyZero() solved the jump issue as well.
/*FVector V = GetVelocity();
if (CapsuleComponent && V.IsNearlyZero())
{
	CapsuleComponent->AddForceAtLocation(V.Size() * V.GetSafeNormal(), GetActorLocation());
}*/

//Camera zoom in and out test using fov
/*	for (FollowCamera->FieldOfView; FollowCamera->FieldOfView == 100.0f; FollowCamera->FieldOfView++);
	for (FollowCamera->FieldOfView; FollowCamera->FieldOfView == 90.0f; FollowCamera->FieldOfView--);*/

//Camera zoom code example from stackoverflow

/*const float CurrentFOV = FollowCamera->FieldOfView;
	const float InterpSpeed = 1.0f;
	UWorld* World = GetWorld();
	FollowCamera->FieldOfView = FMath::FInterpTo(CurrentFOV, 110.0f, World->GetTimeSeconds(), InterpSpeed);*/



// PREVIOUS "MOVEMENT" CODE WITH SIMULATE PHYSICS TURNED ON

	//Move Forward
	/*const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	//CapsuleComponent->AddLocalRotation(FRotator(0,0,0)); // Tested this line to see if it would rotate
	CapsuleComponent->AddForceAtLocationLocal(Direction * 5 * axis * speed, FVector::ZeroVector);*/

	//Move LeftRight
	/*const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//take a look at this code specifically, might have to do something about the drag force or something.
	//the reason why it's able to move mid air is because it's not colliding with anything, but it might be applying force to push it back when it's on ground.
	CapsuleComponent->AddForceAtLocationLocal(Direction * 5 * axis * speed, FVector::ZeroVector);*/

	//Physics Jump
	/*
	void APhysicsPawn::physicsJump()
	{
		//Replace the physicsJump with CheckJump later on.
		isJumping = true;
		const FVector Up = CapsuleComponent->GetUpVector();
		CapsuleComponent->AddImpulse(Up * jumpForce * CapsuleComponent->GetMass());
	}
	*/

	//Check Jump
	/*void APhysicsPawn::CheckJump()
		{
			if (isJumping == true && "Jump")
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("You can't jump midair"));
			}
			else if (isJumping == false && "Jump")
			{
				//physicsJump();

				//isJumping = false;
				//I have to do something similar to isGrounded from unity.
				//I need to set it back to false once it collides with the ground.
			}
		}
		*/

	//Boost
	/*
	 //Basically copied the code from movement functions and edited it to fit the
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//CapsuleComponent->AddImpulse(Forward * pushForce * CapsuleComponent->GetMass());
		CapsuleComponent->AddForceAtLocationLocal(Forward * pushForce * speed, FVector::ZeroVector);
	*/