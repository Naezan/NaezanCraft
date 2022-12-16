#pragma once

float blend(float st, float end, float ratio)
{
    return st + ((end - st) * ratio);
}