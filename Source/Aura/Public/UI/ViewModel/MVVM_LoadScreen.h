// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FText& PlayerName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	void LoadData();

	void SetNumLoadSlots(int32 InNumLoadSlots) { UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumLoadSlots); }
	int32 GetNumLoadSlots() const { return NumLoadSlots; }

private:
	UPROPERTY()
	UMVVM_LoadSlot* SelectedSlot;

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// 下面的声明只是为了被GC管理
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	// 这个变量只是为了让视图模型正常创建，因为没有任何绑定的话，MVVM不会创建
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta =(AllowPrivateAccess="true"))
	int32 NumLoadSlots;
};
