#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Factories/BlueprintFactory.h"
#include "ACFActionsFactory.generated.h"


UCLASS()
class ASCENTEDITOREXTENSIONS_API UACFActionsFactory : public UFactory {
  GENERATED_BODY()

public:
    UACFActionsFactory();

  virtual UObject *FactoryCreateNew(UClass *Class, UObject *InParent,
                                    FName Name, EObjectFlags Flags,
                                    UObject *Context,
                                    FFeedbackContext *Warn) override;

  virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

  /** Returns an optional override brush name for the new asset menu. If this is
   * not specified, the thumbnail for the supported class will be used. */
  virtual FName GetNewAssetThumbnailOverride() const override {
    return "";
  }

  /** Returns the name of the factory for menus */
  virtual FText GetDisplayName() const override { return  FText::FromString("Action"); }

  /** Returns the tooltip text description of this factory */
  virtual FText GetToolTip() const override {
    return  FText::FromString("Creates a new Action");
  }

  /** Returns a new starting point name for newly created assets in the content
   * browser */
  virtual FString GetDefaultNewAssetName() const override {
    return "NewAction";
  }

};
