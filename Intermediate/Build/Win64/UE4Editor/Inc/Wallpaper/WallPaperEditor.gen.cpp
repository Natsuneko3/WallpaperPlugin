// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Wallpaper/Private/WallPaperEditor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWallPaperEditor() {}
// Cross Module References
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperBrush_NoRegister();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperBrush();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_Wallpaper();
// End Cross Module References
	void UWallPaperBrush::StaticRegisterNativesUWallPaperBrush()
	{
	}
	UClass* Z_Construct_UClass_UWallPaperBrush_NoRegister()
	{
		return UWallPaperBrush::StaticClass();
	}
	struct Z_Construct_UClass_UWallPaperBrush_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWallPaperBrush_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_Wallpaper,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperBrush_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "WallPaperEditor.h" },
		{ "ModuleRelativePath", "Private/WallPaperEditor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWallPaperBrush_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWallPaperBrush>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UWallPaperBrush_Statics::ClassParams = {
		&UWallPaperBrush::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UWallPaperBrush_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperBrush_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UWallPaperBrush()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UWallPaperBrush_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UWallPaperBrush, 4001556414);
	template<> WALLPAPER_API UClass* StaticClass<UWallPaperBrush>()
	{
		return UWallPaperBrush::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UWallPaperBrush(Z_Construct_UClass_UWallPaperBrush, &UWallPaperBrush::StaticClass, TEXT("/Script/Wallpaper"), TEXT("UWallPaperBrush"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWallPaperBrush);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
