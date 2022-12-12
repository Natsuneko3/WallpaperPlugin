// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "WallPaperSetting.h"
#include "Styling/SlateBrush.h"
#include "Styling/StyleColors.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWallPaperSetting() {}
// Cross Module References
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FDirectoryPath();
	SLATECORE_API UScriptStruct* Z_Construct_UScriptStruct_FSlateBrush();
	SLATECORE_API UScriptStruct* Z_Construct_UScriptStruct_FStyleColorList();
	UPackage* Z_Construct_UPackage__Script_WallPaper();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperSetting();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperSetting_NoRegister();
// End Cross Module References
	void UWallPaperSetting::StaticRegisterNativesUWallPaperSetting()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UWallPaperSetting);
	UClass* Z_Construct_UClass_UWallPaperSetting_NoRegister()
	{
		return UWallPaperSetting::StaticClass();
	}
	struct Z_Construct_UClass_UWallPaperSetting_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EditorUseGrid_MetaData[];
#endif
		static void NewProp_EditorUseGrid_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_EditorUseGrid;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EditorOpacity_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_EditorOpacity;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_PanelOpacity_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_PanelOpacity;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableTimer_MetaData[];
#endif
		static void NewProp_bEnableTimer_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableTimer;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Time_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Time;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_UseWallpaperEngine_MetaData[];
#endif
		static void NewProp_UseWallpaperEngine_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_UseWallpaperEngine;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_WallPaperDirectoryPath_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_WallPaperDirectoryPath;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Menu_Background_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_Menu_Background;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_StyleColors_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_StyleColors;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWallPaperSetting_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_WallPaper,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "IncludePath", "WallPaperSetting.h" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "UseGrid" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->EditorUseGrid = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid = { "EditorUseGrid", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Editor Opacity" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity = { "EditorOpacity", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, EditorOpacity), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Panel Opacity" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity = { "PanelOpacity", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, PanelOpacity), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Enable Timer" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->bEnableTimer = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer = { "bEnableTimer", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Wallpaper Loop Time(Min)" },
		{ "EditCondition", "bEnableTimer" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time = { "Time", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, Time), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Use Wallpaper Engine" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->UseWallpaperEngine = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine = { "UseWallpaperEngine", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "WallPaper Direction" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath = { "WallPaperDirectoryPath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, WallPaperDirectoryPath), Z_Construct_UScriptStruct_FDirectoryPath, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Menu Background" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background = { "Menu_Background", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, Menu_Background), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData)) }; // 2146961085
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_StyleColors_MetaData[] = {
		{ "Category", "Color Style" },
		{ "DisplayName", "Theme" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_StyleColors = { "StyleColors", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UWallPaperSetting, StyleColors), Z_Construct_UScriptStruct_FStyleColorList, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_StyleColors_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_StyleColors_MetaData)) }; // 762079879
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UWallPaperSetting_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_StyleColors,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWallPaperSetting_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWallPaperSetting>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UWallPaperSetting_Statics::ClassParams = {
		&UWallPaperSetting::StaticClass,
		"Wallpaper",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UWallPaperSetting_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::PropPointers),
		0,
		0x001000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UWallPaperSetting()
	{
		if (!Z_Registration_Info_UClass_UWallPaperSetting.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UWallPaperSetting.OuterSingleton, Z_Construct_UClass_UWallPaperSetting_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UWallPaperSetting.OuterSingleton;
	}
	template<> WALLPAPER_API UClass* StaticClass<UWallPaperSetting>()
	{
		return UWallPaperSetting::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWallPaperSetting);
	UWallPaperSetting::~UWallPaperSetting() {}
	struct Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_Wallpaper_Source_WallPaper_Public_WallPaperSetting_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_Wallpaper_Source_WallPaper_Public_WallPaperSetting_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UWallPaperSetting, UWallPaperSetting::StaticClass, TEXT("UWallPaperSetting"), &Z_Registration_Info_UClass_UWallPaperSetting, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UWallPaperSetting), 3040996601U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_Wallpaper_Source_WallPaper_Public_WallPaperSetting_h_3122095155(TEXT("/Script/WallPaper"),
		Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_Wallpaper_Source_WallPaper_Public_WallPaperSetting_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Editor_Wallpaper_Source_WallPaper_Public_WallPaperSetting_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
