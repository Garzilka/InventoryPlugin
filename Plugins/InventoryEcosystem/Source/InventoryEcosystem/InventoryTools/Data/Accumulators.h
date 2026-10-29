// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Accumulators.generated.h"

UENUM(BlueprintType)
enum class EAccumulatorType : uint8
{
	EAT_Add				UMETA(DisplayName = "Add"),
	EAT_Subtruct		UMETA(DisplayName = "Subtruct"),
	EAT_Multiply		UMETA(DisplayName = "Multiply"),
	EAT_Divide			UMETA(DisplayName = "Divide"),
};

USTRUCT(BlueprintType)
struct FFloatAccumulatorCell
{
	GENERATED_BODY()

public:
	FFloatAccumulatorCell() = default;
	FFloatAccumulatorCell(FName InTag, float InValue)
	{
		Tag = InTag;
		Value = InValue;
	}

	const FName& GetTag() const {return Tag;} ;
	const float& GetValue() const {return Value;} ;

	bool operator==(const FFloatAccumulatorCell& Right) const
	{
		return GetTag() == Right.GetTag();
	}
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Tag = NAME_None; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 1.f;
};

USTRUCT(BlueprintType)
struct FFloatAccumulator
{
	GENERATED_BODY()

public:

	const TArray<FFloatAccumulatorCell>& GetCells() const {return Cells;};

	void AddCell(FName InTag, float InValue)
	{
		FFloatAccumulatorCell* Cell = FindCell(InTag);
	}
	
	FFloatAccumulatorCell* FindCell(FName InTag)
	{
		for (auto Cell : Cells)
		{
			if (Cell.GetTag() != InTag) continue;
			return &Cell;
		}
		return nullptr;
	}
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FFloatAccumulatorCell> Cells;
};










UCLASS()
class INVENTORYTOOLS_API UAccumulators : public UObject
{
	GENERATED_BODY()
	
};
