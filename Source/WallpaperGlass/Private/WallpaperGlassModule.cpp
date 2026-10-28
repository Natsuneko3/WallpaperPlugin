#include "Modules/ModuleManager.h"

#include "Containers/Ticker.h"
#include "Framework/Application/SlateApplication.h"
#include "ISettingsModule.h"
#include "Layout/Children.h"
#include "GlassTextDecoration.h"
#include "GlassWindowBackdrop.h"
#include "Styling/AppStyle.h"
#include "SWallpaperGlassSurface.h"
#include "WallpaperGlassSettings.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "WallpaperGlass"

namespace
{
    struct FGlassTitle
    {
        TWeakPtr<SBorder> Border;
        const FSlateBrush* Original = nullptr;
        FSlateBrush Glass;
    };

    struct FGlassDecoration
    {
        TWeakPtr<SOverlay> Overlay;
        TWeakPtr<SWidget> Node;
        TSharedPtr<SWidget> Original;
        TSharedPtr<SWallpaperGlassSurface> Surface;
        FMargin Padding;
        TArray<TUniquePtr<FGlassTitle>> Titles;
        FGlassTextDecoration Text;

        bool IsAttached() const
        {
            const auto Owner = Overlay.Pin();
            if (!Owner) return false;
            FChildren* Children = Owner->GetChildren();
            for (int32 Index = 0; Index < Children->Num(); ++Index)
            {
                if (Children->GetChildAt(Index) == Surface) return true;
            }
            return false;
        }

        void Restore()
        {
            Text.Restore();
            if (IsAttached())
            {
                const auto Owner = Overlay.Pin();
                Surface->DetachFallback();
                FChildren* Children = Owner->GetChildren();
                for (int32 Index = 0; Index < Children->Num(); ++Index)
                {
                    if (Children->GetChildAt(Index) == Surface)
                    {
                        // UE exposes const slot access even on mutable panels. AttachWidget performs
                        // normal reparenting/invalidation while preserving all slot attributes/order.
                        const_cast<FSlotBase&>(Children->GetSlotAt(Index)).AttachWidget(Original.ToSharedRef());
                        break;
                    }
                }
            }
            for (const auto& Title : Titles)
            {
                if (const auto Border = Title->Border.Pin())
                {
                    if (Border->GetBorderImage() == &Title->Glass) Border->SetBorderImage(Title->Original);
                }
            }
        }
        void UpdateTitles()
        {
            if (const auto Body = Overlay.Pin())
            {
                Text.Update(Body.ToSharedRef(), Surface.ToSharedRef());
            }
            for (const auto& Title : Titles)
            {
                if (const auto Border = Title->Border.Pin())
                {
                    const FSlateBrush* Current = Border->GetBorderImage();
                    if (Current == Title->Original || Current == &Title->Glass)
                    {
                        Border->SetBorderImage(Surface->IsGlassReady() ? &Title->Glass : Title->Original);
                    }
                }
            }
        }
        ~FGlassDecoration() { Restore(); }
    };

    TSharedPtr<SOverlay> FindBody(const TSharedRef<SWidget>& Widget)
    {
        FChildren* Children = Widget->GetChildren();
        // UE 5.6 standard material node body: image, then the content vertical box.
        if (Widget->GetType() == TEXT("SOverlay") && Children->Num() == 2
            && Children->GetChildAt(0)->GetType() == TEXT("SImage")
            && Children->GetChildAt(1)->GetType() == TEXT("SVerticalBox"))
        {
            return StaticCastSharedRef<SOverlay>(Widget);
        }
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            if (auto Found = FindBody(Children->GetChildAt(Index))) return Found;
        }
        return nullptr;
    }

    void DecorateTitles(const TSharedRef<SWidget>& Widget, FGlassDecoration& Decoration)
    {
        if (Widget->GetType() == TEXT("SBorder"))
        {
            auto Border = StaticCastSharedRef<SBorder>(Widget);
            if (Border->GetBorderImage() == FAppStyle::GetBrush(TEXT("Graph.Node.ColorSpill")))
            {
                auto Title = MakeUnique<FGlassTitle>();
                Title->Border = Border;
                Title->Original = Border->GetBorderImage();
                Title->Glass = *Title->Original;
                FLinearColor Tint = Title->Glass.TintColor.GetSpecifiedColor();
                Tint.A *= 0.22f;
                Title->Glass.TintColor = Tint;
                Decoration.Titles.Add(MoveTemp(Title));
            }
        }
        FChildren* Children = Widget->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index)
        {
            DecorateTitles(Children->GetChildAt(Index), Decoration);
        }
    }
}

class FWallpaperGlassModule : public IModuleInterface
{
    friend class FGlassDecorationTest;
    friend class FGlassEditorIntegrationTest;
public:
    virtual void StartupModule() override
    {
        if (IsRunningCommandlet() || !FSlateApplication::IsInitialized()) return;
        FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("WallPaper"));
        Materials = MakeShared<FGlassMaterialCache>();
        if (auto* Settings = FModuleManager::LoadModulePtr<ISettingsModule>(TEXT("Settings")))
        {
            Settings->RegisterSettings(TEXT("Editor"), TEXT("Plugins"), TEXT("WallpaperGlass"),
                LOCTEXT("Name", "Wallpaper Glass"), LOCTEXT("Description", "Video-backed glass for material graph nodes."),
                GetMutableDefault<UWallpaperGlassSettings>());
        }
        ScanTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FWallpaperGlassModule::Scan), 0.5f);
    }

    virtual void ShutdownModule() override
    {
        FTSTicker::RemoveTicker(ScanTicker);
        Decorations.Empty();
        WindowBackdrops.Empty();
        Materials.Reset();
        if (auto* Settings = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
        {
            Settings->UnregisterSettings(TEXT("Editor"), TEXT("Plugins"), TEXT("WallpaperGlass"));
        }
    }

    // Slate widgets must be removed before module code can be unloaded.
    virtual bool SupportsDynamicReloading() override { return false; }

private:
    bool Scan(float)
    {
        if (!GetDefault<UWallpaperGlassSettings>()->bEnabled)
        {
            Decorations.Empty();
            WindowBackdrops.Empty();
            return true;
        }
		Decorations.RemoveAll([](const auto& Item)
		{
			return !Item->Node.IsValid() || !Item->IsAttached();
		});
        for (const auto& Item : Decorations) Item->UpdateTitles();
        TArray<TSharedRef<SWindow>> Windows;
        FSlateApplication::Get().GetAllVisibleWindowsOrdered(Windows);
        for (const auto& Window : Windows) Visit(Window, Window);
		ReconcileWindowBackdrops();
        return true;
    }

	bool RebindDecoration(const TSharedRef<SWidget>& Node, const TSharedRef<SWindow>& Window)
	{
		const auto* Existing = Decorations.FindByPredicate([&](const auto& Item)
		{
			return Item->Node.Pin() == Node;
		});
		if (!Existing)
		{
			return false;
		}
		(*Existing)->Surface->RebindWindow(Window);
		return true;
	}

	void EnsureWindowBackdrop(const TSharedRef<SWindow>& Window)
	{
		if (WindowBackdrops.ContainsByPredicate([&](const auto& Item)
		{
			return Item->BelongsTo(Window);
		}))
		{
			return;
		}
		auto Backdrop = MakeUnique<FGlassWindowBackdrop>(Window);
		if (Backdrop->HasWindow())
		{
			WindowBackdrops.Add(MoveTemp(Backdrop));
		}
	}

	void ReconcileWindowBackdrops()
	{
		TArray<TSharedRef<SWindow>> RequiredWindows;
		for (const auto& Decoration : Decorations)
		{
			if (!Decoration->Node.IsValid() || !Decoration->IsAttached())
			{
				continue;
			}
			if (const auto Window = Decoration->Surface->GetOwnerWindow())
			{
				RequiredWindows.AddUnique(Window.ToSharedRef());
			}
		}
		// Hidden tabs keep attached nodes. Only release their backdrop when the
		// last node detaches, is destroyed, or moves to a different window.
		WindowBackdrops.RemoveAll([&](const auto& Backdrop)
		{
			return !RequiredWindows.ContainsByPredicate([&](const auto& Window)
			{
				return Backdrop->BelongsTo(Window);
			});
		});
		for (const auto& Window : RequiredWindows)
		{
			EnsureWindowBackdrop(Window);
		}
	}

    void Visit(const TSharedRef<SWidget>& Widget, const TSharedRef<SWindow>& Window)
    {
        const FName Type = Widget->GetType();
        if (Type == TEXT("SWallpaperGlassSurface")) return;
        if (Type == TEXT("SGraphNodeMaterialBase") || Type == TEXT("SGraphNodeMaterialCustom")
            || Type == TEXT("SGraphNodeMaterialResult") || Type == TEXT("SGraphNodeMaterialComposite"))
        {
			if (RebindDecoration(Widget, Window))
			{
				return;
			}
            if (auto Body = FindBody(Widget)) Attach(Body.ToSharedRef(), Widget, Window);
            return;
        }
        FChildren* Children = Widget->GetChildren();
        for (int32 Index = 0; Index < Children->Num(); ++Index) Visit(Children->GetChildAt(Index), Window);
    }

    void Attach(const TSharedRef<SOverlay>& Body, const TSharedRef<SWidget>& Node, const TSharedRef<SWindow>& Window)
    {
		if (RebindDecoration(Node, Window))
		{
			return;
		}
		EnsureWindowBackdrop(Window);
        auto Decoration = MakeUnique<FGlassDecoration>();
        Decoration->Overlay = Body;
        Decoration->Node = Node;
        Decoration->Original = Body->GetChildren()->GetChildAt(0);
        const auto& Slot = static_cast<const SOverlay::FOverlaySlot&>(Body->GetChildren()->GetSlotAt(0));
        Decoration->Padding = Slot.GetPadding();
        FSlotBase& MutableSlot = const_cast<FSlotBase&>(Body->GetChildren()->GetSlotAt(0));
        MutableSlot.DetachWidget();
        SAssignNew(Decoration->Surface, SWallpaperGlassSurface).Window(Window).Materials(Materials)
            [Decoration->Original.ToSharedRef()];
        MutableSlot.AttachWidget(Decoration->Surface.ToSharedRef());
        DecorateTitles(Body, *Decoration);
        Decoration->Text.Update(Body, Decoration->Surface.ToSharedRef());
        Decorations.Add(MoveTemp(Decoration));
    }

    FTSTicker::FDelegateHandle ScanTicker;
    TSharedPtr<FGlassMaterialCache> Materials;
    TArray<TUniquePtr<FGlassDecoration>> Decorations;
    TArray<TUniquePtr<FGlassWindowBackdrop>> WindowBackdrops;
};

IMPLEMENT_MODULE(FWallpaperGlassModule, WallpaperGlass)
#undef LOCTEXT_NAMESPACE

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "IMaterialEditor.h"
#include "MaterialEditorModule.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/Texture2D.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassDecorationTest, "Wallpaper.Glass.DecorationRestoration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

namespace
{
	int32 CountGlassBackdrops(const TSharedRef<SWidget>& Widget)
	{
		int32 Count = Widget->GetType() == TEXT("SGlassWindowBackdrop") ? 1 : 0;
		FChildren* Children = Widget->GetChildren();
		for (int32 Index = 0; Index < Children->Num(); ++Index)
		{
			Count += CountGlassBackdrops(Children->GetChildAt(Index));
		}
		return Count;
	}
}

bool FGlassDecorationTest::RunTest(const FString&)
{
    FWallpaperGlassModule Module;
    Module.Materials = MakeShared<FGlassMaterialCache>();
    const auto Window = SNew(SWindow);
    const auto Original = SNew(SImage);
    const auto Content = SNew(SVerticalBox);
    const auto Body = SNew(SOverlay)
        + SOverlay::Slot().Padding(4)[Original]
        + SOverlay::Slot()[Content];
    TestTrue(TEXT("Recognize the UE standard body layout"), FindBody(Body) == Body);
    Module.Attach(Body, Body, Window);
    TestEqual(TEXT("Decoration preserves the slot count"), Body->GetNumWidgets(), 2);
    TestTrue(TEXT("Original foreground remains above glass"), Body->GetChildren()->GetChildAt(1) == Content);
    TestTrue(TEXT("Replacement is attached"), Module.Decorations[0]->IsAttached());
	TestEqual(TEXT("A decorated window has one backdrop"), CountGlassBackdrops(Window), 1);
	const auto SurfaceBeforeMove = Module.Decorations[0]->Surface;
	const auto MovedWindow = SNew(SWindow);
	Module.Attach(Body, Body, MovedWindow);
	Module.ReconcileWindowBackdrops();
	TestEqual(TEXT("Moving the same node does not add another decoration"), Module.Decorations.Num(), 1);
	TestTrue(TEXT("Moving a node preserves its existing glass surface"),
		Module.Decorations[0]->Surface == SurfaceBeforeMove);
	TestTrue(TEXT("Moving a node rebinds the video mapping window"),
		SurfaceBeforeMove->GetOwnerWindow() == MovedWindow);
	TestEqual(TEXT("Moving the last node removes its old window backdrop"), CountGlassBackdrops(Window), 0);
	TestEqual(TEXT("The new window receives one backdrop"), CountGlassBackdrops(MovedWindow), 1);
	Body->SetVisibility(EVisibility::Collapsed);
	Module.ReconcileWindowBackdrops();
	TestEqual(TEXT("A hidden attached tab retains its window backdrop"), CountGlassBackdrops(MovedWindow), 1);
	Body->SetVisibility(EVisibility::Visible);
    Module.Decorations.Empty();
	Module.ReconcileWindowBackdrops();
	TestEqual(TEXT("Removing the last decoration removes its window backdrop"), CountGlassBackdrops(MovedWindow), 0);
    TestTrue(TEXT("Original background restored"), Body->GetChildren()->GetChildAt(0) == Original);
    const auto& RestoredSlot = static_cast<const SOverlay::FOverlaySlot&>(Body->GetChildren()->GetSlotAt(0));
    TestTrue(TEXT("Original padding restored"), RestoredSlot.GetPadding() == FMargin(4));

    Module.Attach(Body, Body, Window);
    const auto Replacement = SNew(SImage);
    const_cast<FSlotBase&>(Body->GetChildren()->GetSlotAt(0)).AttachWidget(Replacement);
    TestFalse(TEXT("Node reconstruction invalidates decoration"), Module.Decorations[0]->IsAttached());
    Module.Decorations.Empty();
	Module.ReconcileWindowBackdrops();
    TestTrue(TEXT("Restoration does not overwrite rebuilt content"), Body->GetChildren()->GetChildAt(0) == Replacement);
	TestEqual(TEXT("Rebuilt nodes do not leave window backdrops behind"), CountGlassBackdrops(Window), 0);

	TSharedPtr<SBorder> LastNode = SNew(SBorder)[Body];
	Module.Attach(Body, LastNode.ToSharedRef(), Window);
	TestEqual(TEXT("The final live node installs a backdrop"), CountGlassBackdrops(Window), 1);
	LastNode.Reset();
	Module.ReconcileWindowBackdrops();
	TestEqual(TEXT("Closing the last node removes the backdrop even if its body is cached"), CountGlassBackdrops(Window), 0);
	Module.Decorations.Empty();
	TestTrue(TEXT("A cached body is restored after its node is closed"), Body->GetChildren()->GetChildAt(0) == Replacement);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassEditorIntegrationTest, "Wallpaper.Glass.EditorIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassEditorIntegrationTest::RunTest(const FString&)
{
    struct FBackgroundFixture
    {
        FWindowStyle& Style = const_cast<FWindowStyle&>(FAppStyle::Get().GetWidgetStyle<FWindowStyle>(TEXT("Window")));
        FSlateBrush OriginalEditor = Style.BackgroundBrush;
        FSlateBrush OriginalPanel = Style.ChildBackgroundBrush;
        TStrongObjectPtr<UTexture2D> Texture;
        FBackgroundFixture()
            : Texture(LoadObject<UTexture2D>(nullptr, TEXT("/WallPaper/WallPaperEngine/Backgound/wallhaven-966dxk.wallhaven-966dxk")))
        {
            if (Texture.IsValid())
            {
                Style.BackgroundBrush.SetResourceObject(Texture.Get());
                Style.ChildBackgroundBrush.SetResourceObject(Texture.Get());
            }
        }
        ~FBackgroundFixture()
        {
            Style.BackgroundBrush = OriginalEditor;
            Style.ChildBackgroundBrush = OriginalPanel;
        }
    };
    auto Background = MakeShared<FBackgroundFixture>();
    auto Material = MakeShared<TStrongObjectPtr<UMaterial>>(NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient));
    auto* Expression = NewObject<UMaterialExpressionConstant3Vector>(Material->Get());
    Expression->Constant = FLinearColor(0.1f, 0.5f, 0.9f);
    Expression->MaterialExpressionEditorX = -240;
    Material->Get()->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(Expression);
    Material->Get()->GetEditorOnlyData()->BaseColor.Connect(0, Expression);
    Material->Get()->PostEditChange();
    auto Editor = FModuleManager::LoadModuleChecked<IMaterialEditorModule>(TEXT("MaterialEditor"))
        .CreateMaterialEditor(EToolkitMode::Standalone, nullptr, Material->Get());
    const double Start = FPlatformTime::Seconds();
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Editor, Material, Background, Start]()
    {
        if (FPlatformTime::Seconds() - Start < 8.0) return false;
        auto& Module = FModuleManager::GetModuleChecked<FWallpaperGlassModule>(TEXT("WallpaperGlass"));
        TestTrue(TEXT("Material editor nodes were discovered and decorated"), Module.Decorations.Num() >= 2);
        bool bReady = false;
        bool bHighlightReady = false;
        for (const auto& Decoration : Module.Decorations)
        {
            bReady |= Decoration->Surface->IsGlassReady();
            bHighlightReady |= Decoration->Surface->IsHighlightReady();
        }
        if ((!bReady || !bHighlightReady) && FPlatformTime::Seconds() - Start < 45.0) return false;
        TestTrue(TEXT("Glass material is ready in a real material editor"), bReady);
        TestTrue(TEXT("Cursor highlight is ready in a real material editor"), bHighlightReady);
        if (bReady)
        {
            auto Window = FSlateApplication::Get().FindWidgetWindow(Module.Decorations[0]->Surface.ToSharedRef());
			TestTrue(TEXT("The real material editor window has a clearing backdrop"), Window
				&& Module.WindowBackdrops.ContainsByPredicate([&](const auto& Backdrop)
				{
					return Backdrop->BelongsTo(Window.ToSharedRef());
				}));
            TArray<FColor> Pixels;
            FIntVector Size = FIntVector::ZeroValue;
            if (Window && FSlateApplication::Get().TakeScreenshot(Window.ToSharedRef(), Pixels, Size))
            {
                TArray64<uint8> PNG;
                FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, PNG);
                FFileHelper::SaveArrayToFile(PNG, *(FPaths::ProjectSavedDir() / TEXT("GlassEditorPreview.png")));
            }
        }
        Editor->CloseWindow(EAssetEditorCloseReason::AssetEditorHostClosed);
        return true;
    }));
    return true;
}
#endif
