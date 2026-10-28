#include "GlassWindowBackdrop.h"

#include "Brushes/SlateColorBrush.h"
#include "Rendering/DrawElements.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/SWindow.h"

namespace
{
	class SGlassWindowBackdrop : public SLeafWidget
	{
	public:
		SLATE_BEGIN_ARGS(SGlassWindowBackdrop) {}
		SLATE_END_ARGS()

		void Construct(const FArguments&)
		{
			SetVisibility(EVisibility::HitTestInvisible);
			ForceVolatile(true);
		}

		virtual FVector2D ComputeDesiredSize(float) const override
		{
			return FVector2D::ZeroVector;
		}

		virtual int32 OnPaint(const FPaintArgs&, const FGeometry& Geometry, const FSlateRect&,
			FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle&, bool) const override
		{
			// Ordinary Slate windows load the previous backbuffer. Alpha blending the
			// bottom layer would keep old nodes, even when all widgets repaint correctly.
			FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(), &Brush,
				ESlateDrawEffect::NoBlending, FLinearColor::Black);
			return Layer;
		}

	private:
		FSlateColorBrush Brush{FLinearColor::Black};
	};
}

TSharedRef<SWidget> MakeGlassWindowBackdrop()
{
	return SNew(SGlassWindowBackdrop);
}

FGlassWindowBackdrop::FGlassWindowBackdrop(const TSharedRef<SWindow>& InWindow)
{
	// Native/per-pixel transparent windows have their own composition semantics.
	if (!InWindow->HasOverlay() || InWindow->HasOSWindowBorder()
		|| InWindow->GetTransparencySupport() == EWindowTransparency::PerPixel)
	{
		return;
	}
	Window = InWindow;
	Backdrop = MakeGlassWindowBackdrop();
	// -1 means append in SOverlay, so use an explicit negative Z order below zero.
	InWindow->AddOverlaySlot(-100)[Backdrop.ToSharedRef()];
}

FGlassWindowBackdrop::~FGlassWindowBackdrop()
{
	if (const auto Owner = Window.Pin(); Owner && Backdrop)
	{
		Owner->RemoveOverlaySlot(Backdrop.ToSharedRef());
	}
}

bool FGlassWindowBackdrop::BelongsTo(const TSharedRef<SWindow>& InWindow) const
{
	return Window.Pin() == InWindow;
}

bool FGlassWindowBackdrop::HasWindow() const
{
	return Window.IsValid();
}
