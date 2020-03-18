#define RootSignatureDefault \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS)," \
	"CBV(b0, visibility=D3D12_SHADER_VISIBILITY_VERTEX, flags=D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE)" \
	"SRV(t0, visibility=D3D12_SHADER_VISIBILITY_PIXEL, flags=D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE)," \
	"StaticSampler(s0)"

cbuffer uniforms: register(b0) {
  float3x2 transform;
}

Texture2D<min16float4> texture: register(t0);

SamplerState csampler: register(s0);

struct VertexIn {
  float2 pos: SV_Position;
};

struct VertexOut {
  float4 pos: SV_Position;
  float2 uv : TEXCOORD0;
};

[RootSignature(RootSignatureDefault)]
VertexOut vertexPassThrough(VertexIn vertIn) {
  VertexOut vertOut;
  vertOut.pos = float4(mul(uniforms.transform, float3(vertIn[vid].pos, 1.0)).xy, 0.0, 1.0);

  float2 uv = 0.5 * (vertIn[vid].pos.xy + 1.0);
  uv.y = 1.0 - uv.y;
  vertOut.uv = uv;
  return vertOut;
}

min16float4 pixelPassThrough(VertexOut pixIn): SV_Target {
  return texture.Sample(csampler, pixIn.uv);
}
