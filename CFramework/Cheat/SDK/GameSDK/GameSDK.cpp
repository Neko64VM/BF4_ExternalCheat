#include "GameSDK.h"

bool Vec2_Empty(const Vector2& value)
{
    return value == Vector2();
}

bool Vec3_Empty(const Vector3& value)
{
    return value == Vector3();
}

float GetDistance(const Vector3 value1, const Vector3 value2)
{
    float num = value1.x - value2.x;
    float num2 = value1.y - value2.y;
    float num3 = value1.z - value2.z;

    return sqrt(num * num + num2 * num2 + num3 * num3);
}

bool WorldToScreen(Matrix ViewMatrix, RECT Size, Vector3 vIn, Vector2& vOut)
{
    float cX = Size.right * 0.5f;
    float cY = Size.bottom * 0.5f;

    float w = ViewMatrix(0, 3) * vIn.x + ViewMatrix(1, 3) * vIn.y + ViewMatrix(2, 3) * vIn.z + ViewMatrix(3, 3);

    if (w < 0.65f)
        return false;

    float x = ViewMatrix(0, 0) * vIn.x + ViewMatrix(1, 0) * vIn.y + ViewMatrix(2, 0) * vIn.z + ViewMatrix(3, 0);
    float y = ViewMatrix(0, 1) * vIn.x + ViewMatrix(1, 1) * vIn.y + ViewMatrix(2, 1) * vIn.z + ViewMatrix(3, 1);

    vOut.x = cX + cX * x / w;
    vOut.y = cY - cY * y / w;

    return true;
}