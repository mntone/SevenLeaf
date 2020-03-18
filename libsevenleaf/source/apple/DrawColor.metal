#include <metal_stdlib>

using namespace metal;

struct Uniforms {
  float4x4 transform;
  float4 color;
};

struct VertexIn {
  float2 position;
};

struct VertexOut {
  float4 position [[position]];
  float4 color    [[user(color)]];
};

// ---
// DrawColor
// ---
vertex VertexOut DrawColorVS(constant VertexIn *vertIn,
                             constant Uniforms& uniforms [[buffer(16)]],
                             uint vid [[vertex_id]]) {
  VertexOut vertOut;
  vertOut.position = uniforms.transform * float4(vertIn[vid].position, 1.0, 1.0);
  vertOut.color    = uniforms.color;
  return vertOut;
}

vertex VertexOut DrawColorInstancedVS(constant VertexIn *vertIn,
                                      constant Uniforms& uniforms [[buffer(16)]],
                                      constant short2 *offsets [[buffer(17)]],
                                      uint vid [[vertex_id]],
                                      uint iid [[instance_id]]) {
  VertexOut vertOut;
  vertOut.position = uniforms.transform * float4(vertIn[vid].position + float2(offsets[iid]), 1.0, 1.0);
  vertOut.color    = uniforms.color;
  return vertOut;
}

fragment float4 DrawColorPS(VertexOut fragIn [[stage_in]]) {
  return fragIn.color;
}
