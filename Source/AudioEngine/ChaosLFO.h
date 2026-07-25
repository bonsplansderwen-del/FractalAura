#pragma once
#include <cmath>
#include <algorithm>

namespace AuraDSP
{
    class ChaosLFO
    {
    public:
        ChaosLFO() = default;

        void setSpeed(float speedHz) { dt = (speedHz * 0.01f); }

        float getNextSample()
        {
            float dx = sigma * (y - x);
            float dy = x * (rho - z) - y;
            float dz = x * y - beta * z;

            x += dx * dt;
            y += dy * dt;
            z += dz * dt;

            return std::clamp(x * 0.05f, -1.0f, 1.0f);
        }

    private:
        float x { 0.1f }, y { 0.0f }, z { 0.0f };
        float sigma { 10.0f }, rho { 28.0f }, beta { 8.0f / 3.0f };
        float dt { 0.005f };
    };
}
