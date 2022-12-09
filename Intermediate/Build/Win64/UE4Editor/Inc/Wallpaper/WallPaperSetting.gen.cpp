// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Wallpaper/Public/WallPaperSetting.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWallPaperSetting() {}
// Cross Module References
	WALLPAPER_API UScriptStruct* Z_Construct_UScriptStruct_FGraphPanelStyle();
	UPackage* Z_Construct_UPackage__Script_Wallpaper();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FLinearColor();
	WALLPAPER_API UScriptStruct* Z_Construct_UScriptStruct_FUMGEditorPalette();
	SLATECORE_API UScriptStruct* Z_Construct_UScriptStruct_FTableRowStyle();
	WALLPAPER_API UScriptStruct* Z_Construct_UScriptStruct_FEditorCustomizeDetailsView();
	SLATECORE_API UScriptStruct* Z_Construct_UScriptStruct_FSlateBrush();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperSetting_NoRegister();
	WALLPAPER_API UClass* Z_Construct_UClass_UWallPaperSetting();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FDirectoryPath();
// End Cross Module References
class UScriptStruct* FGraphPanelStyle::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern WALLPAPER_API uint32 Get_Z_Construct_UScriptStruct_FGraphPanelStyle_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGraphPanelStyle, Z_Construct_UPackage__Script_Wallpaper(), TEXT("GraphPanelStyle"), sizeof(FGraphPanelStyle), Get_Z_Construct_UScriptStruct_FGraphPanelStyle_Hash());
	}
	return Singleton;
}
template<> WALLPAPER_API UScriptStruct* StaticStruct<FGraphPanelStyle>()
{
	return FGraphPanelStyle::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGraphPanelStyle(FGraphPanelStyle::StaticStruct, TEXT("/Script/Wallpaper"), TEXT("GraphPanelStyle"), false, nullptr, nullptr);
static struct FScriptStruct_Wallpaper_StaticRegisterNativesFGraphPanelStyle
{
	FScriptStruct_Wallpaper_StaticRegisterNativesFGraphPanelStyle()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GraphPanelStyle")),new UScriptStruct::TCppStructOps<FGraphPanelStyle>);
	}
} ScriptStruct_Wallpaper_StaticRegisterNativesFGraphPanelStyle;
	struct Z_Construct_UScriptStruct_FGraphPanelStyle_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GridLineColor_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GridLineColor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GridRuleColor_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GridRuleColor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GridCenterColor_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GridCenterColor;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGraphPanelStyle>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridLineColor_MetaData[] = {
		{ "Category", "GraphPanelStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridLineColor = { "GridLineColor", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGraphPanelStyle, GridLineColor), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridLineColor_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridLineColor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridRuleColor_MetaData[] = {
		{ "Category", "GraphPanelStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridRuleColor = { "GridRuleColor", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGraphPanelStyle, GridRuleColor), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridRuleColor_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridRuleColor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridCenterColor_MetaData[] = {
		{ "Category", "GraphPanelStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridCenterColor = { "GridCenterColor", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGraphPanelStyle, GridCenterColor), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridCenterColor_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridCenterColor_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridLineColor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridRuleColor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::NewProp_GridCenterColor,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_Wallpaper,
		nullptr,
		&NewStructOps,
		"GraphPanelStyle",
		sizeof(FGraphPanelStyle),
		alignof(FGraphPanelStyle),
		Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGraphPanelStyle()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGraphPanelStyle_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_Wallpaper();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GraphPanelStyle"), sizeof(FGraphPanelStyle), Get_Z_Construct_UScriptStruct_FGraphPanelStyle_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGraphPanelStyle_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGraphPanelStyle_Hash() { return 1380325570U; }
class UScriptStruct* FUMGEditorPalette::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern WALLPAPER_API uint32 Get_Z_Construct_UScriptStruct_FUMGEditorPalette_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FUMGEditorPalette, Z_Construct_UPackage__Script_Wallpaper(), TEXT("UMGEditorPalette"), sizeof(FUMGEditorPalette), Get_Z_Construct_UScriptStruct_FUMGEditorPalette_Hash());
	}
	return Singleton;
}
template<> WALLPAPER_API UScriptStruct* StaticStruct<FUMGEditorPalette>()
{
	return FUMGEditorPalette::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FUMGEditorPalette(FUMGEditorPalette::StaticStruct, TEXT("/Script/Wallpaper"), TEXT("UMGEditorPalette"), false, nullptr, nullptr);
static struct FScriptStruct_Wallpaper_StaticRegisterNativesFUMGEditorPalette
{
	FScriptStruct_Wallpaper_StaticRegisterNativesFUMGEditorPalette()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("UMGEditorPalette")),new UScriptStruct::TCppStructOps<FUMGEditorPalette>);
	}
} ScriptStruct_Wallpaper_StaticRegisterNativesFUMGEditorPalette;
	struct Z_Construct_UScriptStruct_FUMGEditorPalette_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_UMGEditor_PaletteHeader_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_UMGEditor_PaletteHeader;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_UMGEditor_PaletteItem_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_UMGEditor_PaletteItem;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FUMGEditorPalette>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteHeader_MetaData[] = {
		{ "Category", "UMGEditorPalette" },
		{ "DisplayName", "UMGEditor.PaletteHeader" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteHeader = { "UMGEditor_PaletteHeader", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FUMGEditorPalette, UMGEditor_PaletteHeader), Z_Construct_UScriptStruct_FTableRowStyle, METADATA_PARAMS(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteHeader_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteHeader_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteItem_MetaData[] = {
		{ "Category", "UMGEditorPalette" },
		{ "DisplayName", "UMGEditor.PaletteItem" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteItem = { "UMGEditor_PaletteItem", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FUMGEditorPalette, UMGEditor_PaletteItem), Z_Construct_UScriptStruct_FTableRowStyle, METADATA_PARAMS(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteItem_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteItem_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteHeader,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::NewProp_UMGEditor_PaletteItem,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_Wallpaper,
		nullptr,
		&NewStructOps,
		"UMGEditorPalette",
		sizeof(FUMGEditorPalette),
		alignof(FUMGEditorPalette),
		Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FUMGEditorPalette()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FUMGEditorPalette_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_Wallpaper();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("UMGEditorPalette"), sizeof(FUMGEditorPalette), Get_Z_Construct_UScriptStruct_FUMGEditorPalette_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FUMGEditorPalette_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FUMGEditorPalette_Hash() { return 703443824U; }
class UScriptStruct* FEditorCustomizeDetailsView::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern WALLPAPER_API uint32 Get_Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView, Z_Construct_UPackage__Script_Wallpaper(), TEXT("EditorCustomizeDetailsView"), sizeof(FEditorCustomizeDetailsView), Get_Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Hash());
	}
	return Singleton;
}
template<> WALLPAPER_API UScriptStruct* StaticStruct<FEditorCustomizeDetailsView>()
{
	return FEditorCustomizeDetailsView::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FEditorCustomizeDetailsView(FEditorCustomizeDetailsView::StaticStruct, TEXT("/Script/Wallpaper"), TEXT("EditorCustomizeDetailsView"), false, nullptr, nullptr);
static struct FScriptStruct_Wallpaper_StaticRegisterNativesFEditorCustomizeDetailsView
{
	FScriptStruct_Wallpaper_StaticRegisterNativesFEditorCustomizeDetailsView()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("EditorCustomizeDetailsView")),new UScriptStruct::TCppStructOps<FEditorCustomizeDetailsView>);
	}
} ScriptStruct_Wallpaper_StaticRegisterNativesFEditorCustomizeDetailsView;
	struct Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryTop_Hovered_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryTop_Hovered;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryTop_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryTop;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryMiddle_Highlighted_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryMiddle_Highlighted;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryMiddle_Hovered_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryMiddle_Hovered;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryMiddle_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryMiddle;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CollapsedCategory_Hovered_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CollapsedCategory_Hovered;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CollapsedCategory_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CollapsedCategory;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CategoryBottom_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CategoryBottom;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AdvancedDropdownBorder_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_AdvancedDropdownBorder;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FEditorCustomizeDetailsView>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_Hovered_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_Hovered = { "CategoryTop_Hovered", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryTop_Hovered), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_Hovered_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_Hovered_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop = { "CategoryTop", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryTop), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Highlighted_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Highlighted = { "CategoryMiddle_Highlighted", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryMiddle_Highlighted), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Highlighted_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Highlighted_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Hovered_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Hovered = { "CategoryMiddle_Hovered", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryMiddle_Hovered), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Hovered_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Hovered_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle = { "CategoryMiddle", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryMiddle), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_Hovered_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_Hovered = { "CollapsedCategory_Hovered", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CollapsedCategory_Hovered), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_Hovered_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_Hovered_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory = { "CollapsedCategory", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CollapsedCategory), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryBottom_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryBottom = { "CategoryBottom", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, CategoryBottom), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryBottom_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryBottom_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_AdvancedDropdownBorder_MetaData[] = {
		{ "Category", "DetailsViewStyle" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_AdvancedDropdownBorder = { "AdvancedDropdownBorder", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FEditorCustomizeDetailsView, AdvancedDropdownBorder), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_AdvancedDropdownBorder_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_AdvancedDropdownBorder_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop_Hovered,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryTop,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Highlighted,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle_Hovered,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryMiddle,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory_Hovered,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CollapsedCategory,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_CategoryBottom,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::NewProp_AdvancedDropdownBorder,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_Wallpaper,
		nullptr,
		&NewStructOps,
		"EditorCustomizeDetailsView",
		sizeof(FEditorCustomizeDetailsView),
		alignof(FEditorCustomizeDetailsView),
		Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FEditorCustomizeDetailsView()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_Wallpaper();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("EditorCustomizeDetailsView"), sizeof(FEditorCustomizeDetailsView), Get_Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FEditorCustomizeDetailsView_Hash() { return 1691230588U; }
	void UWallPaperSetting::StaticRegisterNativesUWallPaperSetting()
	{
	}
	UClass* Z_Construct_UClass_UWallPaperSetting_NoRegister()
	{
		return UWallPaperSetting::StaticClass();
	}
	struct Z_Construct_UClass_UWallPaperSetting_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_UseWallpaperEngine_MetaData[];
#endif
		static void NewProp_UseWallpaperEngine_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_UseWallpaperEngine;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EditorUseGrid_MetaData[];
#endif
		static void NewProp_EditorUseGrid_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_EditorUseGrid;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EditorOpacity_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_EditorOpacity;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PanelOpacity_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_PanelOpacity;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bEnableTimer_MetaData[];
#endif
		static void NewProp_bEnableTimer_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEnableTimer;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Time_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Time;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_WallPaperDirectoryPath_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_WallPaperDirectoryPath;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Menu_Background_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Menu_Background;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWallPaperSetting_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_Wallpaper,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "WallPaperSetting.h" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Use Wallpaper Engine" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->UseWallpaperEngine = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine = { "UseWallpaperEngine", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "UseGrid" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->EditorUseGrid = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid = { "EditorUseGrid", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Editor Opacity" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity = { "EditorOpacity", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWallPaperSetting, EditorOpacity), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Panel Opacity" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity = { "PanelOpacity", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWallPaperSetting, PanelOpacity), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Enable Timer" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	void Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_SetBit(void* Obj)
	{
		((UWallPaperSetting*)Obj)->bEnableTimer = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer = { "bEnableTimer", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UWallPaperSetting), &Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Wallpaper Loop Time(Min)" },
		{ "EditCondition", "bEnableTimer" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time = { "Time", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWallPaperSetting, Time), METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "WallPaper Direction" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath = { "WallPaperDirectoryPath", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWallPaperSetting, WallPaperDirectoryPath), Z_Construct_UScriptStruct_FDirectoryPath, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData[] = {
		{ "Category", "Engine Style" },
		{ "DisplayName", "Menu Background" },
		{ "ModuleRelativePath", "Public/WallPaperSetting.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background = { "Menu_Background", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWallPaperSetting, Menu_Background), Z_Construct_UScriptStruct_FSlateBrush, METADATA_PARAMS(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UWallPaperSetting_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_UseWallpaperEngine,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorUseGrid,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_EditorOpacity,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_PanelOpacity,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_bEnableTimer,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Time,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_WallPaperDirectoryPath,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWallPaperSetting_Statics::NewProp_Menu_Background,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWallPaperSetting_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWallPaperSetting>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UWallPaperSetting_Statics::ClassParams = {
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
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UWallPaperSetting_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UWallPaperSetting, 3427339859);
	template<> WALLPAPER_API UClass* StaticClass<UWallPaperSetting>()
	{
		return UWallPaperSetting::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UWallPaperSetting(Z_Construct_UClass_UWallPaperSetting, &UWallPaperSetting::StaticClass, TEXT("/Script/Wallpaper"), TEXT("UWallPaperSetting"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWallPaperSetting);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
