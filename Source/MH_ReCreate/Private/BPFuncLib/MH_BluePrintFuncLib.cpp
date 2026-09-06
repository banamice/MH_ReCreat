// Fill out your copyright notice in the Description page of Project Settings.


#include "MH_ReCreate/Public/BPFuncLib/MH_BluePrintFuncLib.h"

#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogMH);

void UMH_BluePrintFuncLib::PrintStr(const FLinearColor& Color, const FString& Content, const FName Key)
{
	const FString LogMessage = Key.IsNone()
		? Content
		: FString::Printf(TEXT("[%s] %s"), *Key.ToString(), *Content);

	UE_LOG(LogMH, Log, TEXT("%s"), *LogMessage);

	if (GEngine)
	{
		const int32 ScreenMessageKey = Key.IsNone()
			? INDEX_NONE
			: static_cast<int32>(GetTypeHash(Key));

		GEngine->AddOnScreenDebugMessage(
			ScreenMessageKey,
			2.0f,
			Color.ToFColor(true),
			Content);
	}
}
