#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Enums.h"

class Light
{
public:
	virtual ~Light() = default;

	virtual Elite::RGBColor CalculateIllumination(const Elite::FPoint3& hitPoint) const = 0;
	virtual bool IsOutOfRange(const Elite::FPoint3& hitPoint) const = 0;
	virtual Elite::FVector3 GetDirection(const Elite::FPoint3& lookAt) const = 0;
	LightType GetType() const;
	bool IsOn() const;
	void SetIsOn(bool isOn);

protected:
	explicit Light(float intensity, const Elite::RGBColor& color, LightType type, bool isOn = true);

	LightType m_Type;
	Elite::RGBColor m_Color;
	float m_Intensity;
	bool m_IsOn;
};

