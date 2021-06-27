// Copyright 2021 Mickael Daniel. All Rights Reserved.


#include "UI/AGRUWDebugWidget.h"

#include "AGRExtensionsLog.h"
#include "Components/AGRAnimMasterComponent.h"
#include "Animation/AGRCoreAnimInstance.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Character.h"


void UAGRUWDebugWidget::SetupOwner(APlayerController* OwningPlayer)
{
	if (!OwningPlayer)
	{
		AGR_ERROR(TEXT("UAGRUWDebugWidget::SetupOwner passed in PlayerController is null"))
		return;
	}

	OwnerPlayerController = OwningPlayer;
	OwnerCharacter = Cast<ACharacter>(OwningPlayer->GetPawn());
	OwnerAnimMaster = OwnerCharacter->FindComponentByClass<UAGRAnimMasterComponent>();
	OwnerAnimInstance = Cast<UAGRCoreAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	AnimClass = OwnerAnimInstance->GetClass();
}

void UAGRUWDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupOwner(GetOwningPlayer());

	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT %s"), *OwnerPlayerController->GetName())
	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT %s"), *OwnerCharacter->GetName())
	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT %s"), *OwnerAnimMaster->GetName())
	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT %s"), *OwnerAnimInstance->GetName())

	if (bPropsInitialized)
	{
		return;
	}

	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT ---------- Props -----------"))

	TArray<FAGRDebugProp> DebugProps;
	GetDebugProps(DebugProps);

	for (FAGRDebugProp DebugProp : DebugProps)
	{
		FProperty* Prop = DebugProp.Property;
		FString CPPType = Prop->GetCPPType();
		const TMap<FName, FString>* MetaDataMap = Prop->GetMetaDataMap();

		AGR_LOG(Log, TEXT("    Prop %s (%s) - Type: %s"), *Prop->GetName(), *DebugProp.Category, *CPPType);
		AGR_LOG(Log, TEXT("    Prop Class: %s"), *Prop->GetOwnerClass()->GetName());
		if (MetaDataMap)
		{
			for (const TPair<FName, FString> MetaData : *MetaDataMap)
			{
				AGR_LOG(Log, TEXT("    Metadata %s: %s"), *MetaData.Key.ToString(), *MetaData.Value);
			}
		}

		// Custom child Anim BP props
		if (Prop->GetOwnerClass() != UAGRCoreAnimInstance::StaticClass() && AnimBPCustomPropsVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AnimBPCustomPropsVerticalBox->AddChildToVerticalBox(Box);
			CustomProps.Add(DebugProp);
		}

		// Anim States
		if (DebugProp.Category == "AGR|Anim States" && AGRAnimStatesVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRAnimStatesVerticalBox->AddChildToVerticalBox(Box);
			AnimStateProps.Add(DebugProp);
		}

		// AimOffset
		if (DebugProp.Category == "AGR|AimOffset" && AGRAimOffsetVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRAimOffsetVerticalBox->AddChildToVerticalBox(Box);
			AimOffsetProps.Add(DebugProp);
		}

		// Rotation
		if (DebugProp.Category == "AGR|Rotation" && AGRRotationVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRRotationVerticalBox->AddChildToVerticalBox(Box);
			RotationProps.Add(DebugProp);
		}

		// Runtime
		if (DebugProp.Category == "AGR|Runtime" && AGRRuntimeVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRRuntimeVerticalBox->AddChildToVerticalBox(Box);
			RuntimeProps.Add(DebugProp);
		}

		// Movement
		if (DebugProp.Category == "AGR|Movement" && AGRMovementVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRMovementVerticalBox->AddChildToVerticalBox(Box);
			MovementProps.Add(DebugProp);
		}

		// State
		if (DebugProp.Category == "AGR|State" && AGRStateVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRStateVerticalBox->AddChildToVerticalBox(Box);
			StateProps.Add(DebugProp);
		}

		// Leans
		if (DebugProp.Category == "AGR|Leans" && AGRLeansVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRLeansVerticalBox->AddChildToVerticalBox(Box);
			LeansProps.Add(DebugProp);
		}

		// Setup
		if (DebugProp.Category == "AGR|Setup" && AGRSetupVerticalBox)
		{
			UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
			AGRSetupVerticalBox->AddChildToVerticalBox(Box);
			SetupProps.Add(DebugProp);
		}
	}

	if (CustomProps.Num() == 0 && AGRDebugPanelCustomProps)
	{
		AGRDebugPanelCustomProps->SetVisibility(ESlateVisibility::Collapsed);
	}

	bPropsInitialized = true;
	AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT ---------- Props -----------"))

}

void UAGRUWDebugWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerAnimInstance.IsValid())
	{
		return;
	}

	if (CustomProps.Num() != 0)
	{
		UpdatePropValues(CustomProps);
	}

	UpdatePropValues(AnimStateProps);
	UpdatePropValues(AimOffsetProps);
	UpdatePropValues(RotationProps);
	UpdatePropValues(RuntimeProps);
	UpdatePropValues(MovementProps);
	UpdatePropValues(StateProps);
	UpdatePropValues(LeansProps);
	UpdatePropValues(SetupProps);
}


void UAGRUWDebugWidget::GetDebugProps(TArray<FAGRDebugProp>& DebugProps)
{
	for (TFieldIterator<FProperty> PropIt(OwnerAnimInstance->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Prop = *PropIt;

		UClass* OwnerClass = Prop->GetOwnerClass();
		if (!OwnerClass->IsChildOf(UAGRCoreAnimInstance::StaticClass()))
		{
			break;
		}

		const FString Category = Prop->GetMetaData("Category");
		if (!Category.IsEmpty())
		{
			DebugProps.Add(FAGRDebugProp(Category, Prop));
		}
	}
}

UHorizontalBox* UAGRUWDebugWidget::CreateHorizontalBox(const FAGRDebugProp DebugProp)
{
	FProperty* const Property = DebugProp.Property;
	if (!Property)
	{
		return nullptr;
	}

	UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>();

	UTextBlock* TextBlockName = NewObject<UTextBlock>();
	TextBlockName->SetText(FText::FromString(Property->GetName()));
	TextBlockName->SetMinDesiredWidth(200.f);
	TextBlockName->SetJustification(ETextJustify::Right);
	FString FontName = TextBlockName->GetDefaultFontName();
	TextBlockName->SetFont(FontInfo);

	UHorizontalBoxSlot* BoxSlot = HorizontalBox->AddChildToHorizontalBox(TextBlockName);
	BoxSlot->SetPadding(FMargin(8.f, 8.f));
	BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	UImage* Image = NewObject<UImage>();
	Image->SetBrushTintColor(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f)));
	Image->SetBrushSize(FVector2D(2.f, 0.f));
	HorizontalBox->AddChildToHorizontalBox(Image);

	UTextBlock* TextBlockValue = NewObject<UTextBlock>();
	TextBlockValue->SetText(FText::FromString("---"));
	TextBlockValue->SetFont(FontInfo);
	TextBlockValue->SetMinDesiredWidth(350.f);
	TextBlockValue->SetJustification(ETextJustify::Center);

	UHorizontalBoxSlot* BoxSlotValue = HorizontalBox->AddChildToHorizontalBox(TextBlockValue);
	BoxSlotValue->SetSize(ESlateSizeRule::Fill);
	BoxSlotValue->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	ValuesMap.Add(Property->GetName(), TextBlockValue);

	return HorizontalBox;
}

void UAGRUWDebugWidget::UpdatePropValues(TArray<FAGRDebugProp> DebugProps)
{
	for (FAGRDebugProp DebugProp : DebugProps)
	{
		FProperty* Property = DebugProp.Property;
		if (!Property)
		{
			continue;
		}

		UTextBlock* TextBlockValue = *ValuesMap.Find(Property->GetName());
		if (TextBlockValue)
		{
			FProperty* AnimProperty = AnimClass->FindPropertyByName(FName(*Property->GetName()));
			FString CPPType = AnimProperty->GetCPPType();

			if (CPPType == "FGameplayTag")
			{
				FGameplayTag* PropPtr = AnimProperty->ContainerPtrToValuePtr<FGameplayTag>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const FGameplayTag PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
				}
			}
			else if (CPPType == "FGameplayTagContainer")
			{
				FGameplayTagContainer* PropPtr = AnimProperty->ContainerPtrToValuePtr<FGameplayTagContainer>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const FGameplayTagContainer PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
				}
			}
			else if (CPPType == "FRotator")
			{
				FRotator* PropPtr = AnimProperty->ContainerPtrToValuePtr<FRotator>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const FRotator PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
				}
			}
			else if (CPPType == "EAimOffsets")
			{
				EAimOffsets* PropPtr = AnimProperty->ContainerPtrToValuePtr<EAimOffsets>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const EAimOffsets PropValue = *PropPtr;
					const FString AimOffsetString = PropValue == EAimOffsets::Aim ? TEXT("Aim") :
						PropValue == EAimOffsets::Look ? TEXT("Look") :
						TEXT("NONE");

					TextBlockValue->SetText(FText::FromString(AimOffsetString));
				}
			}
			else if (CPPType == "EAimOffsetClamp")
			{
				EAimOffsetClamp* PropPtr = AnimProperty->ContainerPtrToValuePtr<EAimOffsetClamp>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const EAimOffsetClamp PropValue = *PropPtr;
					const FString AimOffsetString = PropValue == EAimOffsetClamp::Left ? TEXT("Left") :
						PropValue == EAimOffsetClamp::Nearest ? TEXT("Nearest") :
						PropValue == EAimOffsetClamp::Right ? TEXT("Right") :
						TEXT("NONE");

					TextBlockValue->SetText(FText::FromString(AimOffsetString));
				}
			}
			else if (CPPType == "ERotationMethod")
			{
				ERotationMethod* PropPtr = AnimProperty->ContainerPtrToValuePtr<ERotationMethod>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const ERotationMethod PropValue = *PropPtr;
					const FString AimOffsetString = PropValue == ERotationMethod::AbsoluteRotation ? TEXT("AbsoluteRotation") :
						PropValue == ERotationMethod::DesiredRotation ? TEXT("DesiredRotation") :
						PropValue == ERotationMethod::DesiredAtAngle ? TEXT("DesiredAtAngle") :
						PropValue == ERotationMethod::RotateToVelocity ? TEXT("RotateToVelocity") :
						PropValue == ERotationMethod::NONE ? TEXT("NONE") :
						TEXT("NONE");

					TextBlockValue->SetText(FText::FromString(AimOffsetString));
				}
			}
			else if (CPPType == "TEnumAsByte<EMovementMode>")
			{
				TEnumAsByte<EMovementMode>* PropPtr = AnimProperty->ContainerPtrToValuePtr<TEnumAsByte<EMovementMode>>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const TEnumAsByte<EMovementMode> PropValue = *PropPtr;
					const FString AimOffsetString = PropValue == EMovementMode::MOVE_Custom ? TEXT("Custom") :
						PropValue == EMovementMode::MOVE_Falling ? TEXT("Falling") :
						PropValue == EMovementMode::MOVE_Flying ? TEXT("Flying") :
						PropValue == EMovementMode::MOVE_None ? TEXT("None") :
						PropValue == EMovementMode::MOVE_Swimming ? TEXT("Swimming") :
						PropValue == EMovementMode::MOVE_Walking ? TEXT("Walking") :
						PropValue == EMovementMode::MOVE_NavWalking ? TEXT("Navmesh Walking") :
						PropValue == EMovementMode::MOVE_MAX ? TEXT("MOVE_MAX") :
						TEXT("NONE");

					TextBlockValue->SetText(FText::FromString(AimOffsetString));
				}
			}
			else if (CPPType == "FVector")
			{
				FVector* PropPtr = AnimProperty->ContainerPtrToValuePtr<FVector>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const FVector PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
				}
			}
			else if (CPPType == "FVector2D")
			{
				FVector2D* PropPtr = AnimProperty->ContainerPtrToValuePtr<FVector2D>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const FVector2D PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
				}
			}
			else if (CPPType == "float")
			{
				float* PropPtr = AnimProperty->ContainerPtrToValuePtr<float>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const float PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(FString::SanitizeFloat(PropValue)));
				}
			}
			else if (CPPType == "bool")
			{
				bool* PropPtr = AnimProperty->ContainerPtrToValuePtr<bool>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const bool PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue ? TEXT("true") : TEXT("false")));
					TextBlockValue->SetColorAndOpacity(FSlateColor(PropValue ? GreenColor : RedColor));
				}
			}
			else if (CPPType == "bool")
			{
				bool* PropPtr = AnimProperty->ContainerPtrToValuePtr<bool>(OwnerAnimInstance.Get());
				if (PropPtr)
				{
					const bool PropValue = *PropPtr;
					TextBlockValue->SetText(FText::FromString(PropValue ? TEXT("true") : TEXT("false")));
					TextBlockValue->SetColorAndOpacity(FSlateColor(PropValue ? GreenColor : RedColor));
				}
			}
			else
			{
				FString Str = FString::Format(TEXT("Not Supported - {0}"), { CPPType });
				TextBlockValue->SetText(FText::FromString(Str));
			}
		}
	}
}


