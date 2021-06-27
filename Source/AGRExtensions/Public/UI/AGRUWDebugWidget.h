// Copyright 2021 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AGRUWDebugWidget.generated.h"

class UAGRAnimMasterComponent;
class UAGRCoreAnimInstance;
class UVerticalBox;
class UHorizontalBox;
class UTextBlock;
class UCanvasPanel;

USTRUCT()
struct FAGRDebugProp
{
	GENERATED_BODY()

	FString Category;

	FProperty* Property;

	FAGRDebugProp(const FString& Category, FProperty* PropertyField)
		: Category(Category),
		  Property(PropertyField)
	{
	}

	FAGRDebugProp(): Property(nullptr)
	{
	}
};

UCLASS()
class AGREXTENSIONS_API UAGRUWDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupOwner(APlayerController* OwningPlayer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRVariablesVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRAnimStatesVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRAimOffsetVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRRotationVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRRuntimeVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRMovementVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRStateVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRLeansVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR Extensions|UI")
	UVerticalBox* AGRSetupVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "AGR Extensions|UI")
	UVerticalBox* AnimBPCustomPropsVerticalBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "AGR Extensions|UI")
	UCanvasPanel* AGRDebugPanelCustomProps = nullptr;

	FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12);

	UClass* AnimClass;

	TMap<FString, UTextBlock*> ValuesMap;

	TArray<FAGRDebugProp> CustomProps;
	TArray<FAGRDebugProp> AnimStateProps;
	TArray<FAGRDebugProp> AimOffsetProps;
	TArray<FAGRDebugProp> RotationProps;
	TArray<FAGRDebugProp> RuntimeProps;
	TArray<FAGRDebugProp> MovementProps;
	TArray<FAGRDebugProp> StateProps;
	TArray<FAGRDebugProp> LeansProps;
	TArray<FAGRDebugProp> SetupProps;

	TWeakObjectPtr<ACharacter> OwnerCharacter;
	TWeakObjectPtr<APlayerController> OwnerPlayerController;
	TWeakObjectPtr<UAGRAnimMasterComponent> OwnerAnimMaster;
	TWeakObjectPtr<UAGRCoreAnimInstance> OwnerAnimInstance;

	FLinearColor WhiteColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	FLinearColor GreenColor = FLinearColor(0.729412f, 0.854902f, 0.333333f, 1.f);
	FLinearColor RedColor = FLinearColor(1.f, 0.388235f, 0.278431f, 1.f);

	bool bPropsInitialized = false;

	virtual void GetDebugProps(OUT TArray<FAGRDebugProp>& DebugProps);
	virtual UHorizontalBox* CreateHorizontalBox(FAGRDebugProp DebugProp);
	virtual void UpdatePropValues(TArray<FAGRDebugProp> DebugProps);
};
