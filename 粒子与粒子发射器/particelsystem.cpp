#include "particle_generator.h"
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

using namespace std;
using namespace glm;

GLfloat datas[1080000];

ParticleGenerator::ParticleGenerator(Shader shader, GLuint texture, GLuint amount, vec3 acceleration, GLfloat scale): shader(shader), texture(texture), amount(amount)
{
	this->init(acceleration);
	this->scales = scale;
	this->zhuangji = GL_FALSE;
}

void ParticleGenerator::Update(GLfloat dt, vec3 position, vec3 velocity, GLuint newParticles, vec3 offset)
{
	if (!this->zhuangji)
	{
		for (GLuint i = 0; i < newParticles; ++i)
		{
			int unusedParticle = this->firstUnusedParticle();
			if (unusedParticle != -1)
			{
				this->respawnParticle(this->particles[unusedParticle], position, velocity, offset);
			}
		}
	}

	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= 0.01f; 
		p.Color.a = p.Life;
		if (p.Life > 0.0f)
		{	
			p.Color = vec4(p.Color.r * 0.985f, p.Color.g * (p.Color.a + 0.02f), p.Color.b * (p.Color.a + 0.02f) / 2.0f, p.Color.a);
		}
	}
}

void ParticleGenerator::Draw(mat4 projection, mat4 view, mat4 model, GLuint feedback, GLuint texbuffer)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	//glDisable(GL_LIGHTING);
	//glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	mat4 newview = view;
	swap(newview[0][1], newview[1][0]);
	swap(newview[0][2], newview[2][0]);
	swap(newview[1][2], newview[2][1]);
	newview[0][3] = 0.0f;
	newview[1][3] = 0.0f;
	newview[2][3] = 0.0f;
	newview[3][3] = 1.0f;
	newview[3][0] = 0.0f;
	newview[3][1] = 0.0f;
	newview[3][2] = 0.0f;

	this->shader.Use();
	GLuint total = 0;
	for (vector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
	{
		if (it->Life > 0.0f)
		{	
			glUniform1i(glGetUniformLocation(this->shader.Program, "triangle_count"), 12);
			glUniform1f(glGetUniformLocation(this->shader.Program, "scales"), this->scales);
			glUniform4f(glGetUniformLocation(this->shader.Program, "color"), it->Color.x, it->Color.y, it->Color.z, it->Color.w);
			glUniform3f(glGetUniformLocation(this->shader.Program, "velocity"), it->Velocity.x, it->Velocity.y, it->Velocity.z);
			glUniform3f(glGetUniformLocation(this->shader.Program, "lastposition"), it->Position.x, it->Position.y, it->Position.z);
			glUniform3f(glGetUniformLocation(this->shader.Program, "acceleration"), it->acceleration.x, it->acceleration.y, it->acceleration.z);

			glUniformMatrix4fv(glGetUniformLocation(this->shader.Program, "view"), 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(this->shader.Program, "model"), 1, GL_FALSE, value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(this->shader.Program, "newview"), 1, GL_FALSE, value_ptr(newview));
			glUniformMatrix4fv(glGetUniformLocation(this->shader.Program, "projection"), 1, GL_FALSE, value_ptr(projection));
			glBindTexture(GL_TEXTURE_BUFFER, texbuffer);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->texture);

			glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback, total * 7 * sizeof(GLfloat), 21 * sizeof(GLfloat));//为什么是21的理由：因为绘制的是GL_TRIANGLES，有三个顶点要绘制，需要把这三个顶点的信息都保存下来（虽然我们只需要一个定点的信息就行）
			glBindVertexArray(this->VAO);
			glBeginTransformFeedback(GL_TRIANGLES);
			//glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//glDrawArrays(GL_POINTS, 0, 6);
			glEndTransformFeedback();
			glBindVertexArray(0);

			total++;
		}
	}

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, feedback);
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, total * 7 * sizeof(GLfloat), datas);

	int nums = 0;
	for (vector<Particle>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
	{
		if (it->Life > 0.0f)
		{
			it->Position.x = datas[nums * 7];
			it->Position.y = datas[nums * 7 + 1];
			it->Position.z = datas[nums * 7 + 2];
			it->Velocity.x = datas[nums * 7 + 3];
			it->Velocity.y = datas[nums * 7 + 4];
			it->Velocity.z = datas[nums * 7 + 5];
			if (datas[nums * 7 + 6] == 1.0f)
			{
				this->zhuangji = GL_TRUE;
				it->acceleration -= it->acceleration * 2.0f;
			}
			nums++;
		}
	}

	glUseProgram(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}

void ParticleGenerator::init(vec3 acceleration)
{
	GLuint VBO;
	GLfloat particle_quad[] = 
	{
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	for (GLuint i = 0; i < this->amount; ++i)
	{
		this->particles.push_back(Particle(acceleration));
	}
}

GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) 
	{
		if (this->particles[i].Life <= 0.0f) 
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (GLuint i = 0; i < lastUsedParticle; ++i) 
	{
		if (this->particles[i].Life <= 0.0f) 
		{
			lastUsedParticle = i;
			return i;
		}
	}
	lastUsedParticle = 0;
	return -1;
}

void ParticleGenerator::respawnParticle(Particle &particle, vec3 position,vec3 velocity, vec3 offset)
{
	GLfloat randomx = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomy = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomz = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	
	particle.Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	particle.Life = 1.0f;
	vec3 direction = normalize(vec3(randomx, randomy, randomz));
	particle.Position = position + direction * 0.5f;
	particle.Velocity = (velocity + direction * 2.5f + offset) * 0.1f;
}
