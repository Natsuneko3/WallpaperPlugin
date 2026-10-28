#include "GlassTextDecoration.h"

#include "SWallpaperGlassSurface.h"
#include "WallpaperGlassSettings.h"
#include "Layout/Children.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/SNullWidget.h"

namespace
{
	FSlotBase* FindSlot(FChildren& Children, const TSharedRef<SWidget>& Widget)
	{
		// Some child lists are slotless or expose only a dynamically selected child.
		for (int32 Index = 0; Index < Children.NumSlot(); ++Index)
		{
			const FSlotBase& Slot = Children.GetSlotAt(Index);
			if (Slot.GetWidget() == Widget)
			{
				return &const_cast<FSlotBase&>(Slot);
			}
		}
		return nullptr;
	}

	bool IsTextWidget(const SWidget& Widget)
	{
		const FName Type = Widget.GetType();
		return Type == TEXT("STextBlock") || Type == TEXT("SEditableText")
			|| Type == TEXT("SMultiLineEditableText");
	}

	bool KeepEditingColors(const TSharedRef<SWidget>& Widget)
	{
		if (Widget->GetType() == TEXT("STextBlock"))
		{
			return false;
		}
		if (Widget->HasAnyUserFocusOrFocusedDescendants())
		{
			return true;
		}
		return Widget->GetType() == TEXT("SEditableText")
			? StaticCastSharedRef<SEditableText>(Widget)->AnyTextSelected()
			: StaticCastSharedRef<SMultiLineEditableText>(Widget)->AnyTextSelected();
	}

	class SGlassTextTint : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SGlassTextTint) {}
			SLATE_ARGUMENT(TSharedPtr<SWallpaperGlassSurface>, Surface)
			SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_END_ARGS()

		void Construct(const FArguments& Args)
		{
			Surface = Args._Surface;
			ChildSlot[Args._Content.Widget];
			const TWeakPtr<SWidget> Text = Args._Content.Widget;
			SetVisibility(TAttribute<EVisibility>::CreateLambda([Text]()
			{
				const TSharedPtr<SWidget> Pinned = Text.Pin();
				const EVisibility Visibility = Pinned ? Pinned->GetVisibility() : EVisibility::Collapsed;
				// Preserve collapsed numeric display fields when their editor becomes visible.
				return Visibility == EVisibility::Visible ? EVisibility::SelfHitTestInvisible : Visibility;
			}));
			ForceVolatile(true);
		}

		void DetachText()
		{
			ChildSlot[SNullWidget::NullWidget];
		}

		virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullRect,
			FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style, bool bEnabled) const override
		{
			FWidgetStyle TextStyle = Style;
			const auto Glass = Surface.Pin();
			if (Glass && Glass->IsGlassReady() && GetDefault<UWallpaperGlassSettings>()->bBlackNodeText
				&& !KeepEditingColors(ChildSlot.GetWidget()))
			{
				TextStyle.BlendColorAndOpacityTint(FLinearColor::Black);
			}
			return SCompoundWidget::OnPaint(Args, Geometry, CullRect, Elements, Layer, TextStyle, bEnabled);
		}

	private:
		TWeakPtr<SWallpaperGlassSurface> Surface;
	};
}

FGlassTextDecoration::~FGlassTextDecoration()
{
	Restore();
}

void FGlassTextDecoration::Update(const TSharedRef<SWidget>& Root, const TSharedRef<SWallpaperGlassSurface>& Surface)
{
	Entries.RemoveAll([](const FEntry& Entry)
	{
		const auto Owner = Entry.Owner.Pin();
		return !Owner || !FindSlot(*Owner->GetChildren(), Entry.Wrapper.ToSharedRef());
	});
	Visit(Root, Surface);
}

void FGlassTextDecoration::Visit(const TSharedRef<SWidget>& Owner, const TSharedRef<SWallpaperGlassSurface>& Surface)
{
	if (Owner->GetType() == TEXT("SGlassTextTint") || Owner->GetType() == TEXT("SWallpaperGlassSurface"))
	{
		return;
	}
	FChildren* Children = Owner->GetChildren();
	for (int32 Index = 0; Index < Children->Num(); ++Index)
	{
		const TSharedRef<SWidget> Child = Children->GetChildAt(Index);
		if (!IsTextWidget(*Child))
		{
			Visit(Child, Surface);
			continue;
		}
		FSlotBase* Slot = FindSlot(*Children, Child);
		if (!Slot)
		{
			continue;
		}
		Slot->DetachWidget();
		const auto Wrapper = SNew(SGlassTextTint).Surface(Surface)[Child];
		Slot->AttachWidget(Wrapper);
		Entries.Add({Owner, Child, Wrapper});
	}
}

void FGlassTextDecoration::Restore()
{
	for (const FEntry& Entry : Entries)
	{
		const auto Owner = Entry.Owner.Pin();
		if (!Owner)
		{
			continue;
		}
		if (FSlotBase* Slot = FindSlot(*Owner->GetChildren(), Entry.Wrapper.ToSharedRef()))
		{
			StaticCastSharedPtr<SGlassTextTint>(Entry.Wrapper)->DetachText();
			Slot->AttachWidget(Entry.Original.ToSharedRef());
		}
	}
	Entries.Empty();
}
