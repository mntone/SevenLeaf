#include <metal_stdlib>

using namespace metal;

struct VertexOut {
  float4 position [[position]];
  float4 color    [[user(color)]];
};

struct Uniforms {
  float4x4 transform;
  float4 color;
};

vertex VertexOut vertexColorInput(constant float2 *position,
                                  constant Uniforms& uniforms [[buffer(16)]],
                                  uint vid [[vertex_id]]) {
  VertexOut vertOut;
  vertOut.position = uniforms.transform * float4(position[vid], 1.0, 1.0);
  vertOut.color = uniforms.color; //0.5 * (half4(position[vid].x, 0, position[vid].y, 1) + 1);
  return vertOut;
}

half3 ApplySRGBCurveHalf(half3 x) {
  return select(1.055 * pow(min(x, 1.0), 1.0 / 2.4) - 0.055, 12.92 * max(x, 0.0), x < 0.0031308);
}

half3 RemoveSRGBCurveHalf(half3 x) {
  return select(pow((x + 0.055) / 1.055, 2.4), x / 12.92, x < 0.04045);
}

half3 ApplyCurveSimpleHalf(half3 x, half g) {
  return pow(x, 1.0 / g);
}

half3 RemoveCurveSimpleHalf(half3 x, half g) {
  return pow(x, g);
}

half3 ApplyBT709CurveHalf(half3 x) {
  return select(1.099 * pow(min(x, 1.0), 0.45) - 0.099, 4.500 * max(x, 0.0), x < 0.018);
}

half3 RemoveBT709CurveHalf(half3 x) {
  return select(pow((x + 0.099) / 1.099, 1 / 0.45), x / 4.500, x < 0.0812);
}

half3 ApplyBT1361CurveHalf(half3 x) {
  return select(1.099 * pow(min(x, 1.33), 0.45) - 0.099,
                select(
                       -(1.099 * pow(-4.0 * max(x, -0.25), 0.45) - 0.099) / 4.0,
                       4.500 * x,
                       x < -0.0045),
                x < 0.018);
}

half3 RemoveBT1361CurveHalf(half3 x) {
  return select(pow((x + 0.099) / 1.099, 1 / 0.45), x / 4.500, x < 0.0812);
}

half3 ApplyST2084CurveHalf(half3 x) {
  half m1 = 2610.0 / 4096.0 / 4;
  half m2 = 2523.0 / 4096.0 * 128;
  half c1 = 3424.0 / 4096.0;
  half c2 = 2413.0 / 4096.0 * 32;
  half c3 = 2392.0 / 4096.0 * 32;
  half3 xp = pow(x, m1);
  return pow((c1 + c2 * xp) / (1.0 + c3 * xp), m2);
}

half3 RemoveST2084CurveHalf(half3 x) {
  half m1 = 2610.0 / 4096.0 / 4;
  half m2 = 2523.0 / 4096.0 * 128;
  half c1 = 3424.0 / 4096.0;
  half c2 = 2413.0 / 4096.0 * 32;
  half c3 = 2392.0 / 4096.0 * 32;
  half3 xp = pow(x, 1.0 / m2);
  return pow(max(xp - c1, 0) / (c2 - c3 * xp), 1.0 / m1);
}

half3 BT709toBT2020(half3 x) {
  const half3x3 ConvMat = {
    0.6274039, 0.3292830, 0.0433131,
    0.0690973, 0.9195406, 0.0113623,
    0.0163914, 0.0880133, 0.8955953,
  };
  return ConvMat * x;
}
