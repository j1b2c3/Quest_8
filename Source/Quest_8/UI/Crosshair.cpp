#include "Crosshair.h"

void UCrosshair::UpdateCrosshairSpread(float Spread)
{
	if (CrosshairImage)
	{
		float NewScale = 1.0f + Spread * 0.5f;
		CrosshairImage->SetRenderScale(FVector2D(NewScale, 1.0f));
	}
}
