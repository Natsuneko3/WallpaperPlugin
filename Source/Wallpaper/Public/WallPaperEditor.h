// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "WallPaperEditor.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, hidecategories=(Object))
class WALLPAPER_API UWallPaperBrush : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	const FString AssetPath = "/WallPaper/WallPaperEngine/UMG";
	 void EditorPlay(const FString& Path);
	 void  PanelPlay(const FString& Path);
	
	UMaterial* GetEditorMaterial(){return EditorMatetial;}
	UMaterial* GetPanelMaterial(){return PanelMaterial;}
	UMaterial* GetOpacityZero(){return OpacityZero;}
	
	bool CanPlayvideo(){return CanPlayVideo;}
	void SetCanPlayVideo(bool bPlay);
private:
	bool CanPlayVideo = false;
	
	UMediaPlayer* EditorPlayer;
	UMediaPlayer* PanelPlayer;
	UMediaTexture* EditorTexture;
	UMediaTexture* PanelTexture;
	UMaterial* EditorMatetial;
	UMaterial* PanelMaterial;
	
	
	UMaterial* OpacityZero;

	void CreateEditorMaterial();
	void CreatePanelMaterial();
	void CreateOpacityZeroMaterial();
};
