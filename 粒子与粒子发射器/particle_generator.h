#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL\Shader.h>

using namespace glm;
using namespace std;

struct Particle 
{
	vec3 Position, Velocity, acceleration;
	vec4 Color;
	GLfloat Life;

	Particle(vec3 acceleration)
	{
		this->acceleration = acceleration;
		this->Color = vec4(0.0f);
		this->Life = 0.0f;
		this->Position = vec3(0.0f);
		this->Velocity = vec3(0.0f);
	}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, GLuint texture, GLuint amount, vec3 acceleration, GLfloat scale);
	void Update(GLfloat dt, vec3 position, vec3 velocity, GLuint newParticles, vec3 offset);
	void Draw(mat4 projection, mat4 view, mat4 model, GLuint feedback,GLuint texbuffer);
private:
	vector<Particle> particles;
	GLuint amount;
	Shader shader;
	GLuint texture;
	GLfloat scales;
	GLboolean zhuangji;
	GLuint VAO;
	void init(vec3 acceleration);
	GLuint firstUnusedParticle();
	void respawnParticle(Particle &particle, vec3 position,vec3 velocity, vec3 offset = vec3(0.0f, 0.0f, 0.0f));
};

#endif
