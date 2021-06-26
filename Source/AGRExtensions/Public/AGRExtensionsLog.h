// Copyright 2021 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

AGREXTENSIONS_API DECLARE_LOG_CATEGORY_EXTERN(LogAGRExtensions, Display, All);

#define AGR_LOG(Verbosity, Format, ...) \
{ \
    UE_LOG(LogAGRExtensions, Verbosity, Format, ##__VA_ARGS__); \
}

#define AGR_WARN(Format, ...) \
{ \
    UE_LOG(LogAGRExtensions, Warning, Format, ##__VA_ARGS__); \
}

#define AGR_ERROR(Format, ...) \
{ \
    UE_LOG(LogAGRExtensions, Error, Format, ##__VA_ARGS__); \
}
