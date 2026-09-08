// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MH_AnimNotifyState_PivotFinished.generated.h"

/**
 * Pivot 动画结束窗口的无副作用标记。
 * 该通知状态只用于动画状态机的过渡条件，不修改角色或动画数据。
 */
UCLASS(meta = (DisplayName = "MH Pivot Finished"))
class MH_RECREATE_API UMH_AnimNotifyState_PivotFinished : public UAnimNotifyState
{
	GENERATED_BODY()
};
