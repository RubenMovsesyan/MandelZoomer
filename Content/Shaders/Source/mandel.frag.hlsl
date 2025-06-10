#pragma ps pixelShader

struct VertexToPixel {
    float4 position: POSITION;
    float3 color: COLOR;
};

float4 main(in VertexToPixel IN) : SV_TARGET {
    return float4(IN.color, 1.0);
};
