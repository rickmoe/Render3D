$Shader$	%Vertex%
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_texCoord;

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * position * vec4(-1.0, 1.0, 1.0, 1.0);
	v_texCoord = texCoord;
};

$Shader$	%Fragment%
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
	vec4 texColor = texture2D(u_texture, fract(v_texCoord));
	color = texColor * u_color;
	//color = texColor;
	//color = u_color;
};