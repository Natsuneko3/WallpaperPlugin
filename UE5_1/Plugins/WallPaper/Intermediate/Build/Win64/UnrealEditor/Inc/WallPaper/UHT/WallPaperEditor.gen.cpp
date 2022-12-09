// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "WallPaperEditor.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWallPaperEditor() {}
// Cross Module References
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_WallPaper();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperBrush();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperBrush_NoRegister();
// End Cross Module References
	void UWallPaperBrush::StaticRegisterNativesUWallPaperBrush()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UWallPaperBrush);
	UClass* Z_Construct_UClass_UWallPaperBrush_NoRegister()
	{
		return UWallPaperBrush::StaticClass();
	}
	struct Z_Construct_UClass_UWallPaperBrush_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWallPaperBrush_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_WallPaper,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperBrush_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "WallPaperEditor.h" },
		{ "ModuleRelativePath", "Public/WallPaperEditor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWallPaperBrush_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWallPaperBrush>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UWallPaperBrush_Statics::ClassParams = {
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
		if (!Z_Registration_Info_UClass_UWallPaperBrush.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UWallPaperBrush.OuterSingleton, Z_Construct_UClass_UWallPaperBrush_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UWallPaperBrush.OuterSingleton;
	}
	template<> WALLPAPER_API UClass* StaticClass<UWallPaperBrush>()
	{
		return UWallPaperBrush::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWallPaperBrush);
	UWallPaperBrush::~UWallPaperBrush() {}
	struct Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_WallPaper_Source_WallPaper_Public_WallPaperEditor_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_WallPaper_Source_WallPaper_Public_WallPaperEditor_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UWallPaperBrush, UWallPaperBrush::StaticClass, TEXT("UWallPaperBrush"), &Z_Registration_Info_UClass_UWallPaperBrush, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UWallPaperBrush), 2534279678U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_WallPaper_Source_WallPaper_Public_WallPaperEditor_h_582697623(TEXT("/Script/WallPaper"),
		Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_WallPaper_Source_WallPaper_Public_WallPaperEditor_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_WallPaper_Source_WallPaper_Public_WallPaperEditor_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
