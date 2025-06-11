struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
}

@fragment
fn main(vertex_input: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(vertex_input.uv, 0.0, 1.0);
}
