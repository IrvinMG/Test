#include "ACFActionsSetFactory.h"
#include "Actions/ACFActionsSet.h"
#include "ACFActionsSetBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"





#define LOCTEXT_NAMESPACE "ACFActionsSetFactory"


UACFActionsSetFactory::UACFActionsSetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass =  UACFActionsSetBlueprint::StaticClass(); 
}


UObject* UACFActionsSetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UObject* UACFActionsSetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return CastChecked<UACFActionsSetBlueprint>(FKismetEditorUtilities::CreateBlueprint(UACFActionsSet::StaticClass(), InParent, Name, EBlueprintType::BPTYPE_Normal, UACFActionsSetBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));
}

#undef LOCTEXT_NAMESPACE

