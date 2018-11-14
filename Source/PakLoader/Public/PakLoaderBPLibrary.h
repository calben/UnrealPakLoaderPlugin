// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/PakFile/Public/IPlatformFilePak.h"

#include "PakLoaderBPLibrary.generated.h"


UCLASS()
class PAKLOADER_API UPakLoaderBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	FPakPlatformFile* PakPlatform;

	static bool InitLoaderPakPlatformFile(class FPakPlatformFile* PakPlatform);

	UFUNCTION(BlueprintCallable)
		static void TryLoadingAssetFromPath(FString Path);

	UFUNCTION(BlueprintCallable)
		static void RunLoadingTestOnPakFile(FString PakFilePath);

	UFUNCTION(BlueprintCallable)
		static bool HasPakFileAlreadyBeenLoaded(FString PakFilePath);

	static void TestPakFileIterationAndLoading(FString PakFilePath, FString PakMountDirectory);

	static bool LoadPakFileAndAddToRegistry(class FPakPlatformFile* PakPlatform, FString PakFilePath, FString PakMountDirectory);

	static bool LoadPakFileAndAddToRegistry(FString PakFilePath, FString PakMountDirectory);

};
