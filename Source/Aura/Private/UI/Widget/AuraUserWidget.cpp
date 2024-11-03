// There is no fucking Copyright


#include "UI/Widget/AuraUserWidget.h"

#include "UI/WidgetController/AuraWidgetController.h"

void UAuraUserWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet(WidgetController);
}
