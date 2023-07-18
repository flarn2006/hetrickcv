#pragma once

#include "../HetrickUtilities.hpp"

class HCVScanner
{
public:
    HCVScanner(int _numSteps = 8)
    {
        setNumStages(2);
    }

    void setNumStages(int _numStages)
    {
        numStages = clamp(_numStages, 2, 8);
        invStages = 1.0f/numStages;
        halfStages = numStages * 0.5f;
        remainInvStages = 1.0f - invStages;
    }

    void setWidthParameter(float _width)
    {
        width = clamp(_width * _width, 0.0f, 1.0f) * widthTable[numStages];
    }

    void setScanParameter(float _scan)
    {
        scan = _scan;
    }

    void setSlopeParameter(float _slope)
    {
        slope = _slope;
    }

    void process()
    {
        float scanFactor1 = LERP(width, halfStages, invStages);
        float scanFactor2 = LERP(width, halfStages + remainInvStages, 1.0f);
        float scanFinal = LERP(scan, scanFactor2, scanFactor1);

        float invWidth = 1.0f/(LERP(width, float(numStages), invStages+invStages));

        float subStage = 0.0f;
        for(int i = 0; i < 8; i++)
        {
            inMults[i] = (scanFinal + subStage) * invWidth;
            subStage = subStage - invStages;
        }

        for(int i = 0; i < 8; i++)
        {
            inMults[i] = clamp(inMults[i], 0.0f, 1.0f);
            inMults[i] = triShape(inMults[i]);
            inMults[i] = clamp(inMults[i], 0.0f, 1.0f);

            const float shaped = (2.0f - inMults[i]) * inMults[i];
            inMults[i] = LERP(slope, shaped, inMults[i]);

            outs[i] = ins[i] * inMults[i];
        }
    }

    void setInput(int i, float _value)
    {
        ins[i] = _value;
    }

    float getOutput(int i)
    {
        return outs[i];
    }

    float getMult(int i)
    {
        return inMults[i];
    }

protected:
    float ins[8] = {};
    float outs[8] = {};
    float inMults[8] = {};
    float widthTable[9] = {0, 0, 0, 0.285, 0.285, 0.2608, 0.23523, 0.2125, 0.193};

    float triShape(float _in)
    {
        _in = _in - round(_in);
        return std::abs(_in + _in);
    }

    int numStages = 2;
    float invStages = 0.5;
    float halfStages = 0.5f;
    float remainInvStages = 0.5f;
    float width = 0.5f;
    float scan = 0.5f;
    float slope = 0.5f;
};