#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_color;		

uniform mat4 model_matrix, view_matrix, projection_matrix;
uniform float angle;
uniform int flag_mode;

out vec3 vertex_to_fragment_color;

void main(){

	float z;
	vec3 new_pos = in_position;
	vertex_to_fragment_color = in_color;

	z = in_position.z;
	if(flag_mode==1)
	{

		z = z  + 2*sin(angle +  in_position.x * 3.14152 / 30 + (in_position.y + 1) / 50);
	}
	
	new_pos.z = z;
	gl_Position = projection_matrix*view_matrix*model_matrix*vec4(new_pos,1); 
}
