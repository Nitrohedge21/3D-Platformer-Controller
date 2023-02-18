// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <GameFramework/Character.h>
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
//#include "GameFramework/Controller.h" //Not sure if this is needed so gonna comment it for now.
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>

#include "PhysicsPawn.generated.h"

UCLASS()
class SONIC_PROJECT_API APhysicsPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APhysicsPawn();
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PawnMesh;
	UCapsuleComponent* CapsuleComponent;
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere)
		float speed = 1.0f;
		float maxSpeed = 1.0f;
		float boostSpeed = 1.0f;
		float displayTime = 1.0f;
		float TimeElapsed = 0;


	UPROPERTY(EditAnywhere, Category = "Forces")
		float pushForce = 50.0f;
		float boostForce = 110.0f;
		float stompForce = 2000.0f;
		float jumpDashForce = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Boost Stamina")
		float currentStamina;
		float maxStamina;
		float staminaSprintUsageRate;
		float staminaRechargeRate;
		float delayForStaminaRecharge;
		float canStaminaRecharge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* FollowCamera;
	//Changed the uproperty of the EditDefaultsOnly to be able to edit it through the blueprint.


		void Move_FB(float axis);
		void Move_LR(float axis);
		void BoostLaunch(float axis);
		void boostStart();
		void boostEnd();
		void camReset();
		void checkBoostMeter();
		void Stomp();
		void JumpDash();
		void raycastLine();

	UPROPERTY(EditAnywhere, Category = "States")
		bool isJumping;
		bool isBoosting;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:


};
