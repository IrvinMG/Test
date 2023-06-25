// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "AscentEditorExtensions.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"
#include "Actions/ACFActionsSet.h"
#include "Actions/ACFBaseAction.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "ACFActionsBlueprint.h"
#include "ACFActionsSetBlueprint.h"

#define LOCTEXT_NAMESPACE "FACFEditorExtensions"



class FAssetTypeActions_ACFAction : public FAssetTypeActions_Blueprint
{
public:

	FAssetTypeActions_ACFAction(EAssetTypeCategories::Type InAssetCategory) {
		MyAssetCategory = InAssetCategory;
	 };

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ACFGameObject", "ACF"); }
	virtual UClass* GetSupportedClass() const override { return UACFActionsBlueprint::StaticClass(); }
	virtual uint32 GetCategories() override { return MyAssetCategory; }
	virtual FColor GetTypeColor() const override {
		return FColor(80, 80, 220, 255);
	};


private:
	EAssetTypeCategories::Type MyAssetCategory;
};

class FAssetTypeActions_ACFActionSet : public FAssetTypeActions_Blueprint
{
public:

	FAssetTypeActions_ACFActionSet(EAssetTypeCategories::Type InAssetCategory) {
		MyAssetCategory = InAssetCategory;
	};

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActionSet", "AssetTypeActions_ACFActionsSet", "ACF"); }
	virtual UClass* GetSupportedClass() const override { return UACFActionsSetBlueprint::StaticClass(); }
	virtual uint32 GetCategories() override { return MyAssetCategory; }
	//virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual FColor GetTypeColor() const override {
		return FColor(80, 80, 220, 255);
	};


private:
	EAssetTypeCategories::Type MyAssetCategory;
};




void FAscentEditorExtensions::StartupModule()
{
	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	{
		ACFGraphAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("ACF")), LOCTEXT("GraphAssetCategory", "ACF"));
		TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_ACFAction(ACFGraphAssetCategory));
		TSharedRef<IAssetTypeActions> ActionSet = MakeShareable(new FAssetTypeActions_ACFActionSet(ACFGraphAssetCategory));

		AssetTools.RegisterAssetTypeActions(Action);
		AssetTools.RegisterAssetTypeActions(ActionSet);
	}
}

void FAscentEditorExtensions::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAscentEditorExtensions, AscentEditorExtensions);