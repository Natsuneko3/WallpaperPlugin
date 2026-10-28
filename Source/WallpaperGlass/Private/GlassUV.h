#pragma once

#include "CoreMinimal.h"
#include "Layout/Geometry.h"

struct FGlassUV
{
    FVector2D Origin = FVector2D::ZeroVector;
    FVector2D AxisX = FVector2D::ZeroVector;
    FVector2D AxisY = FVector2D::ZeroVector;

    FVector2D Map(const FVector2D& LocalUV) const
    {
        return Origin + AxisX * LocalUV.X + AxisY * LocalUV.Y;
    }

    static bool Calculate(const FGeometry& Surface, const FGeometry& Background, FGlassUV& Out)
    {
        const FVector2D Size = Background.GetLocalSize();
        if (Size.X <= UE_SMALL_NUMBER || Size.Y <= UE_SMALL_NUMBER)
        {
            return false;
        }
        const auto MapPoint = [&](const FVector2D& Point)
        {
            return FVector2D(Background.AbsoluteToLocal(Surface.LocalToAbsolute(Point))) / Size;
        };
        Out.Origin = MapPoint(FVector2D::ZeroVector);
        Out.AxisX = MapPoint(FVector2D(Surface.GetLocalSize().X, 0)) - Out.Origin;
        Out.AxisY = MapPoint(FVector2D(0, Surface.GetLocalSize().Y)) - Out.Origin;
        return true;
    }
};
