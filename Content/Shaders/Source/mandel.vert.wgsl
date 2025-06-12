    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) uv: vec2<f32>,
    }

    @vertex
    fn main(
        @builtin(vertex_index) vertex_id: u32
    ) -> VertexOutput {
        // let uv: vec2<f32> = vec2<f32>(f32((vertex_id << 1) & 2), f32(vertex_id & 2));
        // let pos = uv * 2.0 - vec2<f32>(1.0, 1.0);

        var output: VertexOutput;
        var pos: vec2<f32>;
        // output.position = vec4<f32>(pos, 0.0, 1.0);
        // output.uv = uv;
        switch (vertex_id) {
        	case 0u: {
                pos.x = -1.0;
                pos.y = -1.0;
                break;
            }
            case 1u: {
                pos.x = -1.0;
                pos.y = 1.0;
                break;
            }
            case 2u: {
                pos.x = 1.0;
                pos.y = -1.0;
                break;
            },
            case 3u: {
                pos.x = 1.0;
                pos.y = -1.0;
                break;
            },
            case 4u: {
                pos.x = -1.0;
                pos.y = 1.0;
                break;
            },
            case 5u: {
                pos.x = 1.0;
                pos.y = 1.0;
                break;
            },
            default: {
	            pos.x = 1.0;
	            pos.y = -1.0;
	           	break;
            }
        }

        output.position = vec4<f32>(pos, 0.0, 1.0);
        output.uv = pos;
        return output;
    }
