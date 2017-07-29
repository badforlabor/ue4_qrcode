// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "QRcode.h"
#include "QRcodeBPLibrary.h"
#include "Interfaces/IImageWrapper.h"
#include "Interfaces/IImageWrapperModule.h"

UQRcodeBPLibrary::UQRcodeBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UTexture2D* UQRcodeBPLibrary::QRcodeGenerate(const FString& txt, bool& IsValid)
{


	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);


	TArray<uint8> RawFileData;
// 	FString FullFilePath = TEXT("d:\\test.bmp");
// 	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath)) return NULL;
	

	int QRGenerator(const char* txt, TArray<uint8>& raw, unsigned int& unWidth);
	unsigned int width = 0;
	QRGenerator(TCHAR_TO_ANSI(*txt), RawFileData, width);

	if (RawFileData.Num() > 0 && ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~
			
			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}

