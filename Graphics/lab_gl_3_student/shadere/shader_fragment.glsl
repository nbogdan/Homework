#version 330
layout(location = 0) out vec4 out_color;

in vec3 vertex_to_fragment_color;

uniform vec4 object_color;
uniform int value;


void main(){

	if(value == 1)
		out_color = 0.8*object_color + 0.2 * vec4(vertex_to_fragment_color,1);
	else
		out_color = vec4(vertex_to_fragment_color,1);
}