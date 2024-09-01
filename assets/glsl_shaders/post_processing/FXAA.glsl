#ifndef FXAA_GLSL
#define FXAA_GLSL

#include "common/functions.glsl"

#define FXAA_QUALITY_SUBPIX 0.75
#define FXAA_QUALITY_EDGE_THRESHOLD 0.15
#define FXAA_QUALITY_EDGE_THRESHOLD_MIN 0.03
#define FXAA_EXTRA_EDGE_SEARCH_STEPS 4
#define FXAA_EDGE_SEARCH_STEP0 1.0
#define FXAA_EXTRA_EDGE_SEARCH_STEP_SIZES 1.5, 2.0, 4.0, 12.0
const float SEARCH_STEPS[FXAA_EXTRA_EDGE_SEARCH_STEPS] = float[FXAA_EXTRA_EDGE_SEARCH_STEPS]( FXAA_EXTRA_EDGE_SEARCH_STEP_SIZES );

vec4 FXAATexOffset(sampler2D tex, vec4 texTexelSize, vec2 uv, vec2 offset)
{
    vec2 rcpPixels = texTexelSize.xy;
    return texture(tex, uv + offset * rcpPixels);
}

vec4 FXAATex(sampler2D tex, vec2 uv)
{
    return texture(tex, uv);
}

vec4 ApplyFXAA(vec4 color, sampler2D sourceTex, vec4 sourceTexelSize, vec2 uv)
{
    vec2 posM = uv;

    // Luminance of the current pixel
    float lumaM = Luminance(color);

    // ----------------
    // 1. Compute the edge
    // ----------------

    // Luminances of the neighboring pixels at the bottom, right, top, and left
    float lumaS = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(0.0, -1.0)));
    float lumaE = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(1.0, 0.0)));
    float lumaN = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(0.0, 1.0)));
    float lumaW = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(-1.0, 0.0)));

    // Calculate the min and max luminances
    float maxSM = max(lumaS, lumaM);
    float minSM = min(lumaS, lumaM);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaW, lumaN);
    float minWN = min(lumaW, lumaN);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);

    // Find the edge
    float rangeMaxScaled = rangeMax * FXAA_QUALITY_EDGE_THRESHOLD;
    float range = rangeMax - rangeMin;

    // Skip the pixel if its neighborhood does not have a high enough contrast
    float rangeMaxClamped = max(FXAA_QUALITY_EDGE_THRESHOLD_MIN, rangeMaxScaled);
    if (range < rangeMaxClamped)
    {
        return color;
    }

    // ----------------
    // 2. Indicate the direction of the edge (horizontal or vertical)
    // ----------------

    // Luminances of the neighboring pixels at the left-top, right-bottom, right-top, and left-bottom
    float lumaNW = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(-1.0, 1.0)));
    float lumaSE = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(1.0, -1.0)));
    float lumaNE = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(1.0, 1.0)));
    float lumaSW = Luminance(FXAATexOffset(sourceTex, sourceTexelSize, posM, vec2(-1.0, -1.0)));

    // Compute the direction of the edge
    // Horizontal weight = abs((N + S) - 2.0 * M)) * 2.0 + abs((NE + SE) - 2.0 * E)) + abs((NW + SW) - 2.0 * W));
    // Vertical weight = abs((W + E) - 2.0 * M)) * 2.0 + abs((NW + NE) - 2.0 * N)) + abs((SW + SE) - 2.0 * S));
    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
    float edgeVert1 = (-2.0 * lumaM) + lumaWE;

    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;

    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = abs(edgeHorz1) * 2.0 + abs(edgeHorz2);
    float edgeVert4 = abs(edgeVert1) * 2.0 + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;

    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;

    // The edge is horizontal or vertical
    bool horzSpan = edgeHorz >= edgeVert;

    // ----------------
    // 3. Compute the subpixel blend factor
    // ----------------
    // Calculate the total luminance of the neighborhood according to weights:
    // 1  2  1
    // 2  M  2
    // 1  2  1
    float subpixNSWE = lumaNS + lumaWE;
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

    // Calculate average of all adjacent neighbors and get the contrast between the middle and this average
    float subpixB = (subpixA * Rcp(12.0)) - lumaM;
    // Normalized the contrast, clamp the result to [0.0, 1.0]
    float subpixC = clamp(abs(subpixB) * Rcp(range), 0.0, 1.0);

    // Compute the subpixel blend factor
    float subpixD = ((-2.0) * subpixC) + 3.0;
    float subpixE = subpixC * subpixC;
    float subpixF = subpixD * subpixE;
    float subpixG = subpixF * subpixF;
    float subpixH = subpixG * FXAA_QUALITY_SUBPIX;

    // ----------------
    // 4. Search the pixels along the edge, and compute the edge blend factor
    // ----------------

    // Determine the one pixel offset distance to the edge
    float lengthSign = sourceTexelSize.x;
    if (horzSpan) lengthSign = sourceTexelSize.y;

    // Indicate the sign of the offset according to the gradients in two opposite directions
    if (!horzSpan) lumaN = lumaE;
    if (!horzSpan) lumaS = lumaW;
    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    if (!pairN) lengthSign = -lengthSign;

    // The start UV coordinate on the edge for searching (The UV coordinate is at the middle of the pixel, so offset by half the one pixel offset distance)
    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    if (!horzSpan) posB.x += lengthSign * 0.5;
    if (horzSpan) posB.y += lengthSign * 0.5;

    // Search one step offset
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : sourceTexelSize.x;
    offNP.y = (horzSpan) ? 0.0 : sourceTexelSize.y;

    // Search in both positive and negative directions
    // The first search in the negative direction
    vec2 posN;
    posN.x = posB.x - offNP.x * FXAA_EDGE_SEARCH_STEP0;
    posN.y = posB.y - offNP.y * FXAA_EDGE_SEARCH_STEP0;
    // The first search in the positive direction
    vec2 posP;
    posP.x = posB.x + offNP.x * FXAA_EDGE_SEARCH_STEP0;
    posP.y = posB.y + offNP.y * FXAA_EDGE_SEARCH_STEP0;

    // Compute the gradient in both directions
    float lumaEndN = Luminance(FXAATex(sourceTex, posN));
    float lumaEndP = Luminance(FXAATex(sourceTex, posP));
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    if (!pairN) lumaNN = lumaSS;
    lumaEndN -= lumaNN * 0.5;
    lumaEndP -= lumaNN * 0.5;

    // The gradient threshold is used to determine if the search has gone off the edge
    float gradient = max(abs(gradientN), abs(gradientS));
    float gradientScaled = gradient * 0.25;

    // If the gradient is greater than the gradient threshold, it means go off the edge, so stop seaching
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    bool doneNP = (!doneN) || (!doneP);

    // If not going off the edge, keep searching
    for (int i = 0; i < FXAA_EXTRA_EDGE_SEARCH_STEPS && doneNP; ++i)
    {
        if (!doneN) posN.x -= offNP.x * SEARCH_STEPS[i];
        if (!doneN) posN.y -= offNP.y * SEARCH_STEPS[i];
        if (!doneP) posP.x += offNP.x * SEARCH_STEPS[i];
        if (!doneP) posP.y += offNP.y * SEARCH_STEPS[i];
        if (!doneN) lumaEndN = Luminance(FXAATex(sourceTex, posN));
        if (!doneP) lumaEndP = Luminance(FXAATex(sourceTex, posP));
        if (!doneN) lumaEndN -= lumaNN * 0.5;
        if (!doneP) lumaEndP -= lumaNN * 0.5;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        doneNP = (!doneN) || (!doneP);
    }

    // The distance from the search start UV coordinate to the search end UV coordinate in both directions
    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan) dstN = posM.y - posN.y;
    if (!horzSpan) dstP = posP.y - posM.y;

    // The gradient of the center pixel in the direction of blending
    float lumaMM = lumaM - lumaNN * 0.5;

    // The sign of the gradient for the search end UV coordinate is opposite to the sign of the gradient for the search start UV coordinate, it means that the search edge has been found
    bool lumaMLTZero = lumaMM < 0.0;
    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;

    // The nearest direction of the two search directions
    bool directionN = dstN < dstP;
    bool goodSpan = directionN ? goodSpanN : goodSpanP;

    // The distance between the end points of the two search directions
    float spanLength = (dstP + dstN);
    float spanLengthRcp = 1.0 / spanLength;

    // Indicate the nearest distance
    float dst = min(dstN, dstP);
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    // Compute the edge blend factor
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;

    // Get the max between the edge blend factor and the subpixel blend factor
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);

    // Compute the final blended UV coordinate
    if (!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
    if (horzSpan) posM.y += pixelOffsetSubpix * lengthSign;

    return vec4(FXAATex(sourceTex, posM).rgb, color.a);
}

#endif