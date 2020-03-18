#include <metal_stdlib>

using namespace metal;

struct Uniforms {
  float4x4 transform;
};

struct VertexOut {
  float4 position          [[position]];
  float2 textureCoordinate [[user(texturecoord)]];
};

// ---
// DrawVS
// ---
struct VertexIn {
  float2 position;
  float2 texCoord;
};

vertex VertexOut DrawVS(constant VertexIn *vertIn,
                        constant Uniforms& uniforms [[buffer(16)]],
                        uint vid [[vertex_id]]) {
  const VertexIn currentVertIn = vertIn[vid];

  VertexOut vertOut;
  vertOut.position = uniforms.transform * float4(currentVertIn.position, 1.0, 1.0);
  vertOut.textureCoordinate = currentVertIn.texCoord;
  return vertOut;
}

// ---
// DrawSimpleVS
// ---
struct SimpleVertexIn {
  float2 position;
};

vertex VertexOut DrawSimpleVS(constant SimpleVertexIn *vertIn,
                              constant Uniforms& uniforms [[buffer(16)]],
                              uint vid [[vertex_id]]) {
  const float2 position = vertIn[vid].position;

  VertexOut vertOut;
  vertOut.position = uniforms.transform * float4(position, 1.0, 1.0);

  float2 uv = 0.5 * (position + 1.0);
  uv.y = 1.0 - uv.y;
  vertOut.textureCoordinate = uv;
  return vertOut;
}

// ---
// DrawHalfPS
// ---
fragment half4 DrawHalfPS(VertexOut fragIn [[stage_in]], texture2d<half> texture) {
  constexpr sampler csampler;
  const half4 color = texture.sample(csampler, fragIn.textureCoordinate);
  return color;
}

// ---
// DrawSinglePS
// ---
fragment float4 DrawSinglePS(VertexOut fragIn [[stage_in]], texture2d<float> texture) {
  constexpr sampler csampler;
  const float4 color = texture.sample(csampler, fragIn.textureCoordinate);
  return color;
}
