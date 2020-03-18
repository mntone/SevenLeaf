#include <metal_stdlib>

using namespace metal;

struct Uniforms {
  float strength;
};

kernel void posterization(texture2d<half, access::read>  inTexture,
                          texture2d<half, access::write> outTexture [[texture(16)]],
                          constant Uniforms& uniforms,
                          uint2 gid [[thread_position_in_grid]]) {
  if ((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height())) {
    float strength = uniforms.strength;
    half4 inColor  = inTexture.read(gid);
    half4 outColor = half4(ceil(strength * inColor.rgb) / strength, inColor.a);
    outTexture.write(outColor, gid);
  }
}
