#version 330 core
layout (location = 0) in vec3 Position; 
layout (location = 1) in vec3 normal; 
layout (location = 2) in vec2 texcoords; 

out vec2 TexCoords;
out vec4 ParticleColor;

out float zhuangji;
out vec3 position_out;
out vec3 velocity_out;


uniform mat4 view;
uniform mat4 model;
uniform mat4 newview;
uniform mat4 projection;

uniform vec4 color;

uniform vec3 velocity;
uniform vec3 lastposition;

uniform float scales;
uniform vec3 acceleration;
uniform int triangle_count;

uniform samplerBuffer cubeBuffer;

float time_step = 0.05f;


bool intersect(vec3 orign,vec3 direction,vec3 v0,vec3 v1,vec3 v2,out vec3 point)
{
	vec3 u, v, n;
	vec3 w0, w;
	float r, a, b;

	u = (v1 - v0);
	v = (v2 - v0);
	n = cross(u, v);

	w0 = orign - v0;
	a = -dot(n, w0);
	b = dot(n, direction);

	r = a / b;
	if (r < 0.0 || r > 1.0)
	{
		return false;
	}
	point = orign + r * direction;
	float uu, uv, vv, wu, wv, D;

	uu = dot(u, u);
	uv = dot(u, v);
	vv = dot(v, v);
	w = point - v0;
	wu = dot(w, u);
	wv = dot(w, v);
	D = uv * uv - uu * vv;

	float s, t;

	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)
	{
		return false;
	}
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)
		return false;
	return true;
}

vec3 reflect_vector (vec3 v, vec3 n)
{
	return v - 2.0 * dot(v, n) * n;
}

void main()
{
	zhuangji = 0.0f;
	vec3 new_velocity = velocity + acceleration * time_step;//速度速率
	vec3 new_position = lastposition + new_velocity * time_step;//位置
	vec3 v0, v1, v2;
	vec3 point;

	ParticleColor = color;
	TexCoords = texcoords;

	for (int i = 0; i < triangle_count; i++)
	{
		v0 = texelFetch(cubeBuffer, i * 3).xyz;
		v1 = texelFetch(cubeBuffer, i * 3 + 1).xyz;
		v2 = texelFetch(cubeBuffer, i * 3 + 2).xyz;

		if (intersect(lastposition.xyz, lastposition.xyz - new_position.xyz, v0, v1, v2, point))
		{
			vec3 n = normalize(cross(v1 - v0, v2 - v0));
			new_position = point + reflect_vector(new_position.xyz - point, n);
			vec3 xx = reflect_vector(new_velocity, n);
			new_velocity = 0.8 * vec3(xx.x * 2.0f, 5.0f, xx.z * 2.0f);
			zhuangji = 1.0f;
		}
	}

	velocity_out = new_velocity;
	position_out = new_position;
	gl_Position = projection * view * model * newview * vec4(Position * scales + lastposition, 1.0f);
}