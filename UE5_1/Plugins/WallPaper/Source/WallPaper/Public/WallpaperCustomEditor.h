// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "IDetailCustomization.h"


class  UWallpaperCustomEditor : public IDetailCustomization
{
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder) override;
};
