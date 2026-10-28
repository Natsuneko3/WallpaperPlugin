#pragma once

#include "Math/RandomStream.h"
#include "Sources/WallpaperItem.h"
#include "WallpaperTarget.h"

// Keeps selection tied to resource identity, even when names or list sizes match.
class FWallpaperList
{
public:
	FWallpaperList()
		: RandomStream(FMath::Rand())
	{
	}

	void SetItems(TArray<FWallpaperItem> NewItems)
	{
		Items.Reset(NewItems.Num());
		for (FWallpaperItem& Item : NewItems)
		{
			Items.Add(MakeShared<FWallpaperItem>(MoveTemp(Item)));
		}
		for (TSharedPtr<FWallpaperItem>& Selection : Selections)
		{
			const TSharedPtr<FWallpaperItem>* MatchingItem = Items.FindByPredicate(
				[&Selection](const TSharedPtr<FWallpaperItem>& Item)
				{
					return Selection && Item->ResourcePath == Selection->ResourcePath && Item->bIsVideo == Selection->bIsVideo;
				});
			Selection = MatchingItem ? *MatchingItem : PickRandom();
		}
	}

	const TArray<TSharedPtr<FWallpaperItem>>& GetItems() const
	{
		return Items;
	}
	TSharedPtr<FWallpaperItem> GetSelection(EWallpaperTarget Target) const
	{
		return Selections[static_cast<uint8>(Target)];
	}

	void SelectRandom(EWallpaperTarget Target)
	{
		Selections[static_cast<uint8>(Target)] = PickRandom();
	}

private:
	TSharedPtr<FWallpaperItem> PickRandom()
	{
		return Items.IsEmpty() ? nullptr : Items[RandomStream.RandHelper(Items.Num())];
	}

	FRandomStream RandomStream;
	TArray<TSharedPtr<FWallpaperItem>> Items;
	TSharedPtr<FWallpaperItem> Selections[2];
};
