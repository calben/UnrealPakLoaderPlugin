// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PakLoader.h"

#define LOCTEXT_NAMESPACE "FPakLoaderModule"

DEFINE_LOG_CATEGORY(LogPakLoader);

void FPakLoaderModule::StartupModule()
{
}

void FPakLoaderModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPakLoaderModule, PakLoader)