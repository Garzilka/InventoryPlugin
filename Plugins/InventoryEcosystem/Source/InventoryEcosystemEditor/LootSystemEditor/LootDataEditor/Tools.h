#pragma once
#include "CoreMinimal.h"
#include "Kismet/KismetStringLibrary.h"

enum ECategorySelector
{
	None,
	Settings,
	Preset,
	Data,
};


struct FCategoryObjectData
{
	TArray<FName> ObjectGUIDS;
	TMap<FName, TSharedPtr<FCategoryObjectData>> Children;
};

class Tools
{
public:
	static TArray<FName> GetPathFromString(FString Path)
	{
		TArray<FName> LootCategoryNames;
		FString NextSplit = Path;
		FString Key = "|";
		FString Right = "";
		FString Left = "";
		bool HasNext = false;
		do 
		{
			HasNext = UKismetStringLibrary::Split(NextSplit, Key, Left, Right);
			if (HasNext && Left.Len() > 0)
			{
				LootCategoryNames.Add(FName(Left));
			}
			
			if (!HasNext && Right.Len() > 0)
			{
				LootCategoryNames.Add(FName(Right));
			}
			NextSplit = Right;
		} while	(HasNext);
		
		return LootCategoryNames;
	}
	static TSharedPtr<FCategoryObjectData> AddRoot(TMap<FName, TSharedPtr<FCategoryObjectData>>& CategoryData, FName& RootName)
	{
		auto Root = MakeShared<FCategoryObjectData>();
		CategoryData.Add(RootName, Root);
		return Root;
	}
	
	static TSharedPtr<FCategoryObjectData> AddChild(TMap<FName, TSharedPtr<FCategoryObjectData>>& Root, TArray<FName> PathTo)
	{
		if (PathTo.Num() == 0)
		{
			return nullptr;
		}
		
		TSharedPtr<FCategoryObjectData> HasPath = FindByPath(Root, PathTo);
		if (HasPath.IsValid()) return HasPath;

		TArray<FName> WaysLeft = PathTo;
		TArray<FName> CurrentPath;
		TMap<FName, TSharedPtr<FCategoryObjectData>>* CurrentRoot = &Root;
		
		//Создаем|Ищем категорию
		for (FName Path : PathTo)
		{
			CurrentPath.Add(Path);
			TSharedPtr<FCategoryObjectData> CurrentCategory = FindByPath(*CurrentRoot, {Path});
			
			if (!CurrentCategory.IsValid())
			{
				CurrentCategory = AddRoot(*CurrentRoot, {Path});
			}
			
			WaysLeft.RemoveAt(0);
			if (WaysLeft.Num() == 0) return CurrentCategory;
			
			CurrentRoot = &CurrentCategory.Get()->Children;
		}
		
		return FindByPath(Root, PathTo);
	}
	
	static TSharedPtr<FCategoryObjectData> FindByPath(TMap<FName, TSharedPtr<FCategoryObjectData>>& CategoryData, TArray<FName> CopyPathParts)
	{
		if (CopyPathParts.Num() == 0)
		{
			return nullptr;
		}

		//Тут мы убираем текущий для вызова рекурсии
		TArray<FName> NextPaths = CopyPathParts;
		NextPaths.RemoveAt(0);
		TSharedPtr<FCategoryObjectData>* Current = CategoryData.Find(CopyPathParts[0]);
		if (!Current) return nullptr;
		
		if (NextPaths.Num() == 0 && Current)
		{
			return *Current;
		}
		
		return FindByPath(Current->Get()->Children, NextPaths);
	}
};
