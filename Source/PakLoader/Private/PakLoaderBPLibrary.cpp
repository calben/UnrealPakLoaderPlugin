// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PakLoaderBPLibrary.h"
#include "PakLoader.h"

#include "Runtime/Core/Public/Serialization/Archive.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/AES.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Serialization/ArrayReader.h"


UPakLoaderBPLibrary::UPakLoaderBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UPakLoaderBPLibrary::InitLoaderPakPlatformFile(FPakPlatformFile* PakPlatform)
{
	IPlatformFile* LocalPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	if (LocalPlatformFile != nullptr)
	{
		IPlatformFile* PakPlatformFile = FPlatformFileManager::Get().GetPlatformFile(TEXT("PakFile"));
		PakPlatform->Initialize(LocalPlatformFile, TEXT(""));
		FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);
		return true;
	}
	return false;
}

void UPakLoaderBPLibrary::TryLoadingAssetFromPath(FString Path)
{
	FStringAssetReference AssetPath(Path);
	AssetPath.FixupForPIE();
	UObject* AssetObject = AssetPath.TryLoad();
	if (AssetObject != nullptr)
	{
		UE_LOG(LogPakLoader, Log, TEXT("Loaded asset object %s"), *AssetObject->GetName());
	}
}

void UPakLoaderBPLibrary::RunLoadingTestOnPakFile(FString PakFilePath)
{
	TestPakFileIterationAndLoading(PakFilePath, FPaths::GetBaseFilename(PakFilePath));
}

bool UPakLoaderBPLibrary::HasPakFileAlreadyBeenLoaded(FString PakFilePath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*("/" + FPaths::GetBaseFilename(PakFilePath) + "/")), AssetData, true, false);
	return (AssetData.Num() > 0) ? true : false;
}

void UPakLoaderBPLibrary::TestPakFileIterationAndLoading(FString PakFilePath, FString PakMountDirectory)
{
	if (LoadPakFileAndAddToRegistry(PakFilePath, PakMountDirectory))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByPath(FName(*("/" + FPaths::GetBaseFilename(PakFilePath))), AssetData, true, false);
		for (FAssetData Asset : AssetData) {
			FString Path = Asset.ObjectPath.ToString();
			FStringAssetReference AssetPath(Path);
			AssetPath.FixupForPIE();
			UObject* AssetObject = AssetPath.TryLoad();
			//UObject* AssetObject = Asset.ToSoftObjectPath().TryLoad();
			if (AssetObject != nullptr)
			{
				UE_LOG(LogPakLoader, Log, TEXT("Loaded asset object %s"), *AssetObject->GetName());
			}
		}
	}
}

bool UPakLoaderBPLibrary::LoadPakFileAndAddToRegistry(FPakPlatformFile * PakPlatform, FString PakFilePath, FString PakMountDirectory)
{
	PakMountDirectory = "/" + PakMountDirectory + "/";
	if (PakPlatform != nullptr)
	{
		const int32 PakOrder = 0;

#if WITH_EDITOR
		UE_LOG(LogPakLoader, Error, TEXT("Nah Bub.  I can't mount %s in editor, unfortunately."), *PakFilePath);
		return false;
#else
		if (!PakPlatform->Mount(*PakFilePath, PakOrder, *PakMountDirectory))
		{
			UE_LOG(LogPakLoader, Error, TEXT("Failed to mount package %s"), *PakFilePath);
			return true;
		}
		else
		{
			UE_LOG(LogPakLoader, Log, TEXT("Mounted %s"), *PakFilePath);
			FString PakMountDirectoryContentFolder = "/" + FPaths::GetBaseFilename(PakFilePath) + "/Content/";
			UE_LOG(LogPakLoader, Log, TEXT("Registering content redirect to %s"), *PakMountDirectoryContentFolder);
			FPackageName::RegisterMountPoint("/" + FPaths::GetBaseFilename(PakFilePath) + "/", *PakMountDirectoryContentFolder);

			FArrayReader SerializedAssetData;
			FString AssetBinPath = FPaths::GetPath(PakFilePath) + "/" + FPaths::GetBaseFilename(PakFilePath) + ".bin";
			if (FFileHelper::LoadFileToArray(SerializedAssetData, *(AssetBinPath)))
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
				AssetRegistry.Serialize(SerializedAssetData);
				return true;
			}
			else
			{
				return false;
			}
		}
#endif
	}
	return false;
}

bool UPakLoaderBPLibrary::LoadPakFileAndAddToRegistry(FString PakFilePath, FString PakMountDirectory)
{
	PakMountDirectory = "/" + PakMountDirectory + "/";
	IPlatformFile* LocalPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	if (LocalPlatformFile != nullptr)
	{
		IPlatformFile* PakPlatformFile = FPlatformFileManager::Get().GetPlatformFile(TEXT("PakFile"));
		FPakPlatformFile* PakPlatform = new FPakPlatformFile();
		PakPlatform->Initialize(LocalPlatformFile, TEXT(""));
		FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

		FPakFile* PakFile = new FPakFile(LocalPlatformFile, *PakFilePath, false);
		if (!PakFile->IsValid())
		{
			UE_LOG(LogPakLoader, Log, TEXT("Pak file %s determined invalid."), *PakFilePath);
			return false;
		}

		PakFile->SetMountPoint(*PakMountDirectory);

		const int32 PakOrder = 0;

#if WITH_EDITOR
		UE_LOG(LogPakLoader, Error, TEXT("Nah Bub.  I can't mount %s in editor, unfortunately."), *PakFilePath);
		return false;
#else
		if (!PakPlatform->Mount(*PakFilePath, PakOrder, *PakMountDirectory))
		{
			UE_LOG(LogPakLoader, Error, TEXT("Failed to mount package %s"), *PakFilePath);
			return true;
		}
		else
		{
			UE_LOG(LogPakLoader, Log, TEXT("Mounted %s"), *PakFilePath);
			FString PakMountDirectoryContentFolder = "/" + FPaths::GetBaseFilename(PakFilePath) + "/Content/";
			UE_LOG(LogPakLoader, Log, TEXT("Registering content redirect to %s"), *PakMountDirectoryContentFolder);
			FPackageName::RegisterMountPoint("/" + FPaths::GetBaseFilename(PakFilePath) + "/", *PakMountDirectoryContentFolder);

			FArrayReader SerializedAssetData;
			FString AssetBinPath = FPaths::GetPath(PakFilePath) + "/" + FPaths::GetBaseFilename(PakFilePath) + ".bin";
			if (FFileHelper::LoadFileToArray(SerializedAssetData, *(AssetBinPath)))
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
				AssetRegistry.Serialize(SerializedAssetData);
				return true;
			}
			else
			{
				return false;
			}
		}
#endif
	}
	return false;
}
