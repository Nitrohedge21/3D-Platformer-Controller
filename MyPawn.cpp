// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

//NOTES TO MYSELF
// Locking the pawn rotation stopped the cube from going crazy but the movement is acting weird now.
//Camera does work but I want it to rotate around player rather than rotating the player.

// CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
// The line below is an example to get a component that is created inside the editor.

//The boost kinda works but it is not building the speed but rather changing the speed per input.

//Instead of doing the line in tick, take a look at widgets for HUD and UI stuff.

// Sets default values
AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	speed = 5.0f;
	runSpeed = 10.0f;
	boostSpeed = 50.0f;
	displayTime = 99999999;
	jumpForce = 500.0f;
	isJumping = false;

	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	PawnMesh->SetStaticMesh(MeshRef.Object);
	PawnMesh->SetupAttachment(GetRootComponent());

	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	MovementComponent->MaxWalkSpeed = 1000.0f;
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	//CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// The camera does indeed spawn but it's position is out of map so I will have to find a way to fix that.

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	//CameraBoom->SetRelativeLocationAndRotation(FVector(10.0f, 0.0f, 175.0f), FRotator(0.0f, -20.0f, 0.0f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	
	//Gotta make this display on HUD rather than a display message. The line below crashes the project on load.
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("SPEED: %f"), AMyPawn::speed));
	//Instead of using getVelocyity i had to reference the speed of the pawn. Also had a syntax error in SPEED text part. Had to change the &f into %f to resolve the issue.
}

void AMyPawn::Move_FB(float axis)
{
	if (axis != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, axis);
		
		//FVector Location = GetActorLocation();
		//Location += GetActorForwardVector() * axis * speed;
		//SetActorLocation(Location);
	}
}

void AMyPawn::Move_LR(float axis)
{
	
	if (axis != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, axis);

		//THE LINE ABOVE WORKS PERFECTLY FINE WHEN SIMULATE PHYSICS IS OFF BUT I WANTED TO HAVE PHYSICS IN MY CASE SO IDK WHAT TO DO

		//FVector Location = GetActorLocation();
		//Location += GetActorRightVector() * axis * speed;
		//SetActorLocation(Location);
	}
}

void AMyPawn::boostStart()
{
	//boost start and end needs more work on them, ask Matt for help.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Boosting"));
	//Need to find the equivalent of the addforce from unity to add speed rather than changing the speed.
	speed += 10.0f;

	if (speed > boostSpeed)
	{
		speed = boostSpeed;
	}
}

void AMyPawn::boostEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Running or walking"));
	for (speed; speed >= runSpeed; speed -= 2.5f)
	{
		if (speed > speed)
		{
			speed = speed;
		}
	}
	//might add walkSpeed later on
}


// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	//This part is essentially Update() from Unity. Super::Tick(DeltaTime); seems like it might be FixedUpdate() but gotta research this a bit further to see whether if it is useful or not.
	Super::Tick(DeltaTime);
	/*GEngine->AddOnScreenDebugMessage(-1, displayTime, FColor::Green, FString::Printf(TEXT("SPEED: %f"), AMyPawn::speed));*/
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Move_FB"), this, &AMyPawn::Move_FB);
	PlayerInputComponent->BindAxis(TEXT("Move_LR"), this, &AMyPawn::Move_LR);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &AMyPawn::boostStart);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &AMyPawn::boostEnd);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &AMyPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down", this, &AMyPawn::AddControllerPitchInput);

	//The Jump and stop jump functions seem to be broken when i try to use those functions with the blueprint.
	
}

//The Equivalent of debug.log from unity is GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Input Text"));

//Swapped the capsule collider with pawn mesh so that the collider is the main object ig (Line 14)

//Tried using a while loop to do something but it kept crashing ue5, check the code below.
/*while (&AMyPawn::boostStart)
	{
		speed += 10.0f;
	}*/


//Previous jump functions

/*PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);*/
/*PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);*/

// Tested but failed movement code

/*CurrentVelocity = CapsuleComponent->GetComponentVelocity();
CurrentVelocity.X = FMath::Clamp(axis, -1.0f, 1.0f) * speed;
CurrentVelocity = CapsuleComponent->GetComponentVelocity();
CurrentVelocity.Y = FMath::Clamp(axis, 1.0f, -1.0f) * speed;*/
/*if (!CurrentVelocity.IsZero())
{
	const FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);
}*/

//personal movement code that did not work as intended

/*const FVector ForwardBackward = CapsuleComponent->GetForwardVector();
CapsuleComponent->AddImpulse(ForwardBackward* speed* CapsuleComponent->GetMass());
const FVector LeftRight = CapsuleComponent->GetRightVector();
CapsuleComponent->AddImpulse(LeftRight* speed* CapsuleComponent->GetMass());*/


//void AMyPawn::physicsJump()
//{
//	isJumping = true;
//	const FVector Up = CapsuleComponent->GetUpVector();
//	CapsuleComponent->AddImpulse(Up * jumpForce * CapsuleComponent->GetMass());
//}

//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPawn::CheckJump);

//void AMyPawn::CheckJump()
//{
//	if (isJumping == true && "Jump")
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("You can't jump midair"));
//	}
//	else if (isJumping == false && "Jump")
//	{
//		physicsJump();
//		//isJumping = false;
//		
//		//I have to do something similar to isGrounded from unity.
//		//I need to set it back to false once it collides with the ground.
//	}
//}