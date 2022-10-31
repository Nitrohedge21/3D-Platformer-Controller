// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <GameFramework/Character.h>
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>

#include "MyPawn.generated.h"

UCLASS()
class SONIC_PROJECT_API AMyPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PawnMesh;
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere)
		float speed = 1.0f;
		float runSpeed = 1.0f;
		float boostSpeed = 1.0f;
		float displayTime = 1.0f;
	
	
	UPROPERTY(EditAnywhere, Category = "Forces")
		float jumpForce = 1000.f;

	/*UPROPERTY(EditAnywhere, Category = Forces);
	float Force = 1000.0f;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;
	
	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* FollowCamera;
	//Changed the uproperty of the EditDefaultsOnly to be able to edit it through the blueprint.


	void Move_FB(float axis);
	void Move_LR(float axis);
	void boostStart();
	void boostEnd();
	void physicsJump();
	void CheckJump();

	UPROPERTY()
		bool isJumping;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
