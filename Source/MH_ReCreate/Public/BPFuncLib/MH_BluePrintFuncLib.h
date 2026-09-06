// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MH_BluePrintFuncLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMH, Log, All);

/**
 * Shared Blueprint utilities for MH_ReCreate.
 */
UCLASS()
class MH_RECREATE_API UMH_BluePrintFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MH|Debug")
	static void PrintStr(const FLinearColor& Color, const FString& Content, FName Key = NAME_None);
};
