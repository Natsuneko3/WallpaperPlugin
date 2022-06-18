// Fill out your copyright notice in the Description page of Project Settings.


#include "WallPaperEditor.h"
#include "WindowsTargetSettings.h"
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
	UWindowsTargetSettings* DefaultRHi = GetMutableDefault<UWindowsTargetSettings>();
	if (DefaultRHi->DefaultGraphicsRHI != EDefaultGraphicsRHI::DefaultGraphicsRHI_DX12 )
	{
		CanPlayVideo = true;
		CreateEditorMaterial();
		CreatePanelMaterial();
	}

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
	UWindowsTargetSettings* DefaultRHi = GetMutableDefault<UWindowsTargetSettings>();
	if (DefaultRHi->DefaultGraphicsRHI != EDefaultGraphicsRHI::DefaultGraphicsRHI_DX12 )
	{
		CanPlayVideo = bPlay;
	}
	else
	{
		CanPlayVideo = false;
	}
}

void UWallPaperBrush::CreateEditorMaterial()
{
	if (CanPlayVideo)
	{
		UPackage* EditorTexturePack = CreatePackage( *AssetPath);
		EditorPlayer = NewObject<UMediaPlayer>(EditorTexturePack, FName("EditorPlayer"), RF_MarkAsRootSet);
		if (!EditorPlayer->IsLooping())
		{
			EditorPlayer->SetLooping(true);
		}

		EditorMatetial = NewObject<UMaterial>(EditorTexturePack, FName("EditorMatetial"), RF_MarkAsRootSet);
		//UMaterialExpressionTextureSample* TextureSample = NewObject<UMaterialExpressionTextureSample>(EditorMatetial);
		UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(EditorMatetial);
		TextureExpression->ParameterName = "Texture";
		
		EditorTexture = NewObject<UMediaTexture>(EditorTexturePack, FName("EditorTexture"), RF_MarkAsRootSet);
		EditorTexture->SetDefaultMediaPlayer(EditorPlayer);
		EditorTexture->UpdateResource();
		TextureExpression->Texture = EditorTexture;
		TextureExpression->AutoSetSampleType();
		EditorMatetial->Expressions.Add(TextureExpression);
		EditorMatetial->MaterialDomain = EMaterialDomain::MD_UI;
		EditorMatetial->EmissiveColor.Expression = TextureExpression;

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
		
		PanelMaterial->Expressions.Add(TextureExpression);

		PanelTexture = NewObject<UMediaTexture>(EditorTexturePack, FName("PanelTexture"), RF_MarkAsRootSet);//LoadObject<UMediaTexture>(NULL,TEXT("/WallPaper/WallPaperEngine/UMG/PanelMediaTexture"));
		PanelTexture->SetDefaultMediaPlayer(PanelPlayer);
		PanelTexture->UpdateResource();
		TextureExpression->Texture = PanelTexture;
		TextureExpression->AutoSetSampleType();
		PanelMaterial->MaterialDomain = EMaterialDomain::MD_UI;
		PanelMaterial->EmissiveColor.Expression = TextureExpression;
		PanelMaterial->PostEditChange();
	}


	//PanelMaterialMID = UMaterialInstanceDynamic::Create(PanelMaterial, NULL);
}

void UWallPaperBrush::CreateOpacityZeroMaterial()
{
	UPackage* EditorTexturePack = CreatePackage(nullptr, *AssetPath);
	OpacityZero = NewObject<UMaterial>(EditorTexturePack, FName("OpacityZero"), RF_MarkAsRootSet);
	UMaterialExpressionConstant* Comment = NewObject<UMaterialExpressionConstant>(OpacityZero);
	Comment->R = 0.0f;
	OpacityZero->Expressions.Add(Comment);
	OpacityZero->BlendMode = EBlendMode::BLEND_Translucent;
	OpacityZero->Opacity.Expression = Comment;
	OpacityZero->PostEditChange();
}
#undef LOCTEXT_NAMESPACE
