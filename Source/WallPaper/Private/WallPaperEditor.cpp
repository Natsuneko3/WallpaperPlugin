// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.


#include "WallPaperEditor.h"

#include "MaterialDomain.h"
#include "Components/ComboBoxString.h"
#include "Interfaces/IPluginManager.h"
#include "Materials/MaterialExpressionComment.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "UObject/ConstructorHelpers.h"


#define LOCTEXT_NAMESPACE "FWallPaperModule"


UWallPaperBrush::UWallPaperBrush(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	


	CanPlayVideo = true;
	CreateEditorMaterial();
	CreatePanelMaterial();


	CreateOpacityZeroMaterial();
}

void UWallPaperBrush::EditorPlay(const FString& Path)
{
	if (IsValid(EditorPlayer) && CanPlayVideo)
	{
		EditorPlayer->OpenFile(Path);
	}
}

void UWallPaperBrush::PanelPlay(const FString& Path)
{
	if (IsValid(PanelPlayer) && CanPlayVideo)
	{
		PanelPlayer->OpenFile(Path);
	}
}

void UWallPaperBrush::SetCanPlayVideo(bool bPlay)
{
	
	CanPlayVideo = bPlay;
}

void UWallPaperBrush::CreateEditorMaterial()
{
	if (CanPlayVideo)
	{
		UPackage* EditorTexturePack = CreatePackage(*AssetPath);
		EditorPlayer = NewObject<UMediaPlayer>(EditorTexturePack, FName("EditorPlayer"), RF_MarkAsRootSet);
		if (!EditorPlayer->IsLooping())
		{
			EditorPlayer->SetLooping(true);
		}

		EditorMatetial = NewObject<UMaterial>(EditorTexturePack, FName("EditorMatetial"), RF_MarkAsRootSet);
		//UMaterialExpressionTextureSample* TextureSample = NewObject<UMaterialExpressionTextureSample>(EditorMatetial);
		UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(EditorMatetial);
		TextureExpression->ParameterName = "Texture";

		UMaterialEditorOnlyData* UnrealMaterialEditorOnly = EditorMatetial->GetEditorOnlyData();
		EditorTexture = NewObject<UMediaTexture>(EditorTexturePack, FName("EditorTexture"), RF_MarkAsRootSet);
		EditorTexture->SetDefaultMediaPlayer(EditorPlayer);
		EditorTexture->UpdateResource();
		TextureExpression->Texture = EditorTexture;
		TextureExpression->AutoSetSampleType();
		UnrealMaterialEditorOnly->ExpressionCollection.Expressions.Add(TextureExpression);
		EditorMatetial->MaterialDomain = EMaterialDomain::MD_UI;
		UnrealMaterialEditorOnly->EmissiveColor.Expression = TextureExpression;

		EditorMatetial->PostEditChange();
	}
}

void UWallPaperBrush::CreatePanelMaterial()
{
	if (CanPlayVideo)
	{
		UPackage* EditorTexturePack = CreatePackage(*AssetPath);
		PanelPlayer = NewObject<UMediaPlayer>(EditorTexturePack, FName("PanelPlayer"), RF_MarkAsRootSet);
		if (!PanelPlayer->IsLooping())
		{
			PanelPlayer->SetLooping(true);
		}

		PanelMaterial = NewObject<UMaterial>(EditorTexturePack, FName("PanelMaterial"), RF_MarkAsRootSet);
		UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(PanelMaterial);

		TextureExpression->ParameterName = "Texture";
		UMaterialEditorOnlyData* UnrealMaterialEditorOnly = PanelMaterial->GetEditorOnlyData();
		UnrealMaterialEditorOnly->ExpressionCollection.Expressions.Add(TextureExpression);

		PanelTexture = NewObject<UMediaTexture>(EditorTexturePack, FName("PanelTexture"), RF_MarkAsRootSet);
		//LoadObject<UMediaTexture>(NULL,TEXT("/WallPaper/WallPaperEngine/UMG/PanelMediaTexture"));
		PanelTexture->SetDefaultMediaPlayer(PanelPlayer);
		PanelTexture->UpdateResource();
		TextureExpression->Texture = PanelTexture;
		TextureExpression->AutoSetSampleType();
		PanelMaterial->MaterialDomain = EMaterialDomain::MD_UI;
		UnrealMaterialEditorOnly->EmissiveColor.Expression = TextureExpression;
		PanelMaterial->PostEditChange();
	}


	//PanelMaterialMID = UMaterialInstanceDynamic::Create(PanelMaterial, NULL);
}

void UWallPaperBrush::CreateOpacityZeroMaterial()
{
	UPackage* EditorTexturePack = CreatePackage(*AssetPath);
	OpacityZero = NewObject<UMaterial>(EditorTexturePack, FName("OpacityZero"), RF_MarkAsRootSet);
	UMaterialExpressionConstant* Comment = NewObject<UMaterialExpressionConstant>(OpacityZero);
	Comment->R = 0.0f;
	UMaterialEditorOnlyData* UnrealMaterialEditorOnly = OpacityZero->GetEditorOnlyData();
	UnrealMaterialEditorOnly->ExpressionCollection.Expressions.Add(Comment);
	OpacityZero->BlendMode = EBlendMode::BLEND_Translucent;
	UnrealMaterialEditorOnly->Opacity.Expression = Comment;
	OpacityZero->PostEditChange();
}
#undef LOCTEXT_NAMESPACE
