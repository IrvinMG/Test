#include "ACFActionsFactory.h"
#include "Actions/ACFBaseAction.h"
#include "ACFActionsBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"





#define LOCTEXT_NAMESPACE "ACFActionsFactory"

UACFActionsFactory::UACFActionsFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UACFActionsBlueprint::StaticClass();
}


UObject* UACFActionsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UObject* UACFActionsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return CastChecked<UACFActionsBlueprint>(FKismetEditorUtilities::CreateBlueprint(UACFBaseAction::StaticClass(), InParent, Name, EBlueprintType::BPTYPE_Normal, UACFActionsBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));
}

#undef LOCTEXT_NAMESPACE

