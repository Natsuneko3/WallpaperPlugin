// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#include "WallPaperEditor.h"

#include "Materials/Material.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogWallpaperMedia, Log, All);

UMaterial* UWallPaperBrush::Play(const FString& Path, EWallpaperTarget Target)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}

	FWallpaperMediaChannel& Channel = GetChannel(Target);
	InitializeChannel(Channel);

	FString FullPath = FPaths::ConvertRelativePathToFull(Path);
	FPaths::NormalizeFilename(FullPath);
	if (Channel.CurrentPath == FullPath && !Channel.Player->HasError() && !Channel.Player->IsClosed())
	{
		return Channel.Material;
	}

	Channel.CurrentPath.Reset();
	if (!Channel.Player->OpenFile(FullPath))
	{
		UE_LOG(LogWallpaperMedia, Warning, TEXT("Unable to open wallpaper video: %s"), *FullPath);
		return nullptr;
	}

	Channel.CurrentPath = MoveTemp(FullPath);
	return Channel.Material;
}

void UWallPaperBrush::Stop(EWallpaperTarget Target)
{
	FWallpaperMediaChannel& Channel = GetChannel(Target);
	// During GC, the player may have already performed its own media cleanup.
	if (Channel.Player && !Channel.Player->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed))
	{
		Channel.Player->Close();
	}
	Channel.CurrentPath.Reset();
}

void UWallPaperBrush::StopAll()
{
	Stop(EWallpaperTarget::Editor);
	Stop(EWallpaperTarget::Panel);
}

void UWallPaperBrush::BeginDestroy()
{
	StopAll();
	Super::BeginDestroy();
}

FWallpaperMediaChannel& UWallPaperBrush::GetChannel(EWallpaperTarget Target)
{
	return Target == EWallpaperTarget::Editor ? EditorChannel : PanelChannel;
}

void UWallPaperBrush::InitializeChannel(FWallpaperMediaChannel& Channel)
{
	if (Channel.Player)
	{
		return;
	}

	Channel.Player = NewObject<UMediaPlayer>(this, NAME_None, RF_Transient);
	Channel.Player->PlayOnOpen = true;
	Channel.Player->SetLooping(true);
	Channel.Texture = NewObject<UMediaTexture>(this, NAME_None, RF_Transient);
	Channel.Texture->SetMediaPlayer(Channel.Player);
	Channel.Texture->UpdateResource();
	CreateMaterial(Channel);
}

void UWallPaperBrush::CreateMaterial(FWallpaperMediaChannel& Channel)
{
	Channel.Material = NewObject<UMaterial>(this, NAME_None, RF_Transient);
	Channel.Material->MaterialDomain = MD_UI;
	Channel.Material->BlendMode = BLEND_Translucent;

	UMaterialExpressionTextureSampleParameter2D* TextureExpression = NewObject<UMaterialExpressionTextureSampleParameter2D>(Channel.Material, NAME_None, RF_Transient);
	TextureExpression->ParameterName = TEXT("Texture");
	TextureExpression->Texture = Channel.Texture;
	TextureExpression->AutoSetSampleType();

	UMaterialEditorOnlyData* MaterialData = Channel.Material->GetEditorOnlyData();
	MaterialData->ExpressionCollection.Expressions.Add(TextureExpression);
	MaterialData->EmissiveColor.Expression = TextureExpression;
	// Slate multiplies the material output by the brush tint, including its alpha.
	MaterialData->Opacity.UseConstant = true;
	MaterialData->Opacity.Constant = 1.0f;
	Channel.Material->PostEditChange();
}
