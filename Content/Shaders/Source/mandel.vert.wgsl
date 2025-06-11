struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@vertex
fn main(
    @builtin(vertex_index) vertex_id: u32
) -> VertexOutput {
    let uv: vec2<f32> = vec2<f32>(f32((vertex_id << 1) & 2), f32(vertex_id & 2));
    let pos = uv * 2.0 - vec2<f32>(1.0, 1.0);

    var output: VertexOutput;
    output.position = vec4<f32>(pos, 0.0, 1.0);
    output.uv = uv;

    return output;
}
