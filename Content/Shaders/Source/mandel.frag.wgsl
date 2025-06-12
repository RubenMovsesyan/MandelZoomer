    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) uv: vec2<f32>,
    }

    fn complex_mult(complex1: vec2<f32>, complex2: vec2<f32>) -> vec2<f32> {
    	let a = complex1.x;
     	let b = complex1.y;

     	let c = complex2.x;
      	let d = complex2.y;

      	let real = (a * c) - (b * d);
      	let imag = (a * d) + (b * c);

       	return vec2<f32>(real, imag);
    }

    @fragment
    fn main(vertex_input: VertexOutput) -> @location(0) vec4<f32> {
    	var c: vec2<f32> = vertex_input.uv;

     	var z: vec2<f32> = vec2<f32>(0.0, 0.0);

      	for (var i = 0; i < 32; i++) {
       		z = complex_mult(z, z) + c;
         	if (length(z) >= 2.0) {
          		return vec4<f32>(f32(i) / 32.0, 0.0, f32(i) / 32.0, 1.0);
            }
       	}

    	return vec4<f32>(0.0, 0.0, 0.0, 1.0);
    }
