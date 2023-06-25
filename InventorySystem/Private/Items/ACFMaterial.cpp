// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Items/ACFMaterial.h"
#include "Items/ACFItem.h"
#include "ACFItemTypes.h"

AACFMaterial::AACFMaterial()
{
	ItemInfo.ItemType = EItemType::Material;
	ItemInfo.Name = FText::FromString("Base Material");
}
