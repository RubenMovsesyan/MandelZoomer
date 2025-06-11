struct Output {
    float4 position: SV_Position;
    float2 uv: TEXCOORD0;
};

Output main(uint vertex_id: SV_VertexID) {
    Output output;
    float2 uv = float2((vertex_id << 1) & 2, vertex_id & 2);
    float2 pos = uv * 2.0 - 1.0;

    output.position = float4(pos, 0.0f, 1.0f);
    output.uv = uv;
    return output;
}
