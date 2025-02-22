// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	void InitializeSlot();

	void SetLoadSlotName(const FString& InSlotName);

	FString GetLoadSlotName() const { return LoadSlotName; };

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	FString LoadSlotName;
};
