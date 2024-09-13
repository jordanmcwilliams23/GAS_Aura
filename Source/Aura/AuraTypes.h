// Copyright Jordan McWilliams

#pragma once

#include "CoreMinimal.h"
#include "AuraTypes.generated.h"


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatChangedSignatureDyn, float, NewValue);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFloatChangedSignature, float /* New Value */);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInt32ChangedSignatureDyn, int32, NewValue);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInt32ChangedSignature, int32 /* New Value */);