#pragma once
#include "Runtime/UMG/Public/Components/VerticalBox.h"

class SAdvancedVerticalBox : public SVerticalBox
{
public:
	void RemoveSlot(int32 Idx)
	{
		Children.RemoveAt(Idx);
		Invalidate(EInvalidateWidget::Layout);
	}
};
