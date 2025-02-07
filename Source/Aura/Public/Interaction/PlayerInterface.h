// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	// 增加经验，返回升了多少级
	UFUNCTION(BlueprintNativeEvent)
	int32 AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InPoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InPoints);

	// 处理升级后的动作，如特效等
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* Mat = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
};
