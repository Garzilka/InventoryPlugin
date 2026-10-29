#include "W_BaseArrayElement.h"

void SW_BaseArrayElement::Construct(const FArguments& InArgs)
{
	ElementIdx = InArgs._ElementIdx;
	OnClicked = InArgs._OnClicked;
}
