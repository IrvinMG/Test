// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "Camera/CameraShakeBase.h"
#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>

#include "CCMTypes.generated.h"
/**
 *
 */

USTRUCT(BlueprintType)
struct FCCMCameraMovementSettings : public FTableRowBase {
    GENERATED_BODY()

public:
    FCCMCameraMovementSettings()
    {
        CameraOffset = FVector();
        Shake = UCameraShakeBase::StaticClass();
        FOV = 0.f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float InterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FovInterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    TSubclassOf<UCameraShakeBase> Shake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsWorldShaking"), Category = "Camera")
    float ShakeIntensity = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bShakeLooping = false;

    FORCEINLINE FCCMCameraMovementSettings operator+=(const FCCMCameraMovementSettings& Other)
    {

        this->CameraOffset += Other.CameraOffset;
        this->FOV += Other.FOV;
        this->InterpSpeed = Other.InterpSpeed;
        this->FovInterpSpeed = Other.FovInterpSpeed;

        if (Other.Shake) {
            this->Shake = Other.Shake;
            this->ShakeIntensity = Other.ShakeIntensity;
            this->bShakeLooping = Other.bShakeLooping;
        }

        return *this;
    }

    FORCEINLINE FCCMCameraMovementSettings operator-=(const FCCMCameraMovementSettings& Other)
    {

        this->CameraOffset = this->CameraOffset - Other.CameraOffset;
        this->FOV -= Other.FOV;
        this->InterpSpeed = Other.InterpSpeed;
        this->FovInterpSpeed = Other.FovInterpSpeed;
        if (Other.Shake) {
            this->Shake = nullptr;
            this->bShakeLooping = false;
        }

        return *this;
    }
};

USTRUCT(BlueprintType)
struct FCurrentSequence {
    GENERATED_BODY()

    FCurrentSequence()
    {
        cameraSequenceComp = nullptr;
    }

public:
    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    class UCCMCameraSplineComponent* cameraSequenceComp;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float currentTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float currentSplinePos = 0.f;

    void Reset(UCCMCameraSplineComponent* inSequence)
    {
        cameraSequenceComp = inSequence;
        currentTime = 0.f;
        currentSplinePos = 0.f;
    }
};

USTRUCT(BlueprintType)
struct FCCMSequenceEvent {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bEditCameraSpeed = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditCameraSpeed), Category = "Camera")
    float CameraSpeed = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bEditFov = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditFov), Category = "Camera")
    float CameraFovOffset = 0.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditFov), BlueprintReadWrite, Category = "Camera")
    float FovOffsetInterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bSwitchLookAt = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bSwitchLookAt), BlueprintReadWrite, Category = "Camera")
    FName LookAtPoint;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bSwitchLookAt), BlueprintReadWrite, Category = "Camera")
    float LookAtRotationSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bChangeTimeDilation = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bChangeTimeDilation), BlueprintReadWrite, Category = "Camera")
    float TimeDilatation = 1.0f;
};

USTRUCT(BlueprintType)
struct FCCMCameraSequenceSettings : public FTableRowBase {
    GENERATED_BODY()

public:

    FCCMCameraSequenceSettings() {
        lookAtActor = nullptr;
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraSpeed = 0.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraRotationsSpeed = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraFov = 90.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float FovInterpSpeed = 1.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    AActor* lookAtActor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    FName LookAtPoint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float TimeDilatation = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float BlendSettingsTime = .3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float OutBlendSettings = 1.f;
};

UENUM(BlueprintType)
enum class ELockType : uint8 {
    EYawOnly = 0 UMETA(DisplayName = "Only Lock Yaw"),
    EAllAxis UMETA(DisplayName = "Lock Yaw And Pitch"),
};

UENUM(BlueprintType)
enum class ETargetLockType : uint8 {
    ENone = 0,
    EActor,
    EComponent,
};
