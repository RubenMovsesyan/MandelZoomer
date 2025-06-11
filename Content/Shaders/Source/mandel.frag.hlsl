struct Input {
    float4 position: SV_Position;
    float2 uv: TEXCOORD0;
};

float4 main(in Input input) : SV_TARGET {
    return float4(input.uv, 0.0, 1.0);
};
