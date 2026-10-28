// Copyright Natsu Neko, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WallpaperTarget.h"
#include "WallPaperEditor.generated.h"

class UMaterial;
class UMediaPlayer;
class UMediaTexture;

USTRUCT()
struct FWallpaperMediaChannel
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UMediaPlayer> Player = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMediaTexture> Texture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterial> Material = nullptr;

	FString CurrentPath;
};

UCLASS(BlueprintType, hidecategories = (Object))
class WALLPAPER_API UWallPaperBrush : public UObject
{
	GENERATED_BODY()

public:
	UMaterial* Play(const FString& Path, EWallpaperTarget Target);
	void Stop(EWallpaperTarget Target);
	void StopAll();
	virtual void BeginDestroy() override;

private:
	FWallpaperMediaChannel& GetChannel(EWallpaperTarget Target);
	void InitializeChannel(FWallpaperMediaChannel& Channel);
	void CreateMaterial(FWallpaperMediaChannel& Channel);

	UPROPERTY(Transient)
	FWallpaperMediaChannel EditorChannel;

	UPROPERTY(Transient)
	FWallpaperMediaChannel PanelChannel;
};
