// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

class UAuraWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UAuraWidgetController* InWidgetController);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAuraWidgetController> WidgetController;

	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet(UAuraWidgetController* NewWidgetController);
};
