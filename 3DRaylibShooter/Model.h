#pragma once
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include "Settings.h"
#include <vector>
#include <functional>
#include <memory>

class BaseModel
{
public:
	Vector3 m_pos{};
	Model m_model{};
	bool m_despawn = false;

protected:

	
	Vector3 m_dir{};
	float m_speed{};
	BoundingBox m_box{};
public:  
	
	BaseModel() = default;

	BaseModel(Model model, Vector3 pos, Vector3 dir, float speed) : m_model(model), m_pos(pos), m_dir(dir), m_speed(speed)
	{
		m_box = GetMeshBoundingBox(model.meshes[0]);
	}

	virtual void update(float dt)
	{
		move(dt);
		//check despawn
	}
	virtual void draw()
	{
		DrawModel(m_model, m_pos, 1.f, WHITE);
	}

	
protected:

	virtual void move(float dt)
	{
		m_pos.x += m_dir.x * m_speed * dt;
		m_pos.z += m_dir.z * m_speed * dt;
	}


};
class Floor : public BaseModel
{
public:
	Texture2D texture;

	Floor() = default;

	Floor(Texture2D tex)
	{
		texture = tex;
		m_model = LoadModelFromMesh(GenMeshCube(36.0f, 1.0f, 34.0f));
		m_pos = { 6.5f, -2.8f, -8.0f };
		m_dir = { 0.0f, 0.0f, 0.0f };
		m_speed = 0.0f;

		SetMaterialTexture(&m_model.materials[0], MATERIAL_MAP_ALBEDO, texture);
	}

	void draw() override
	{
		DrawModel(m_model, m_pos, 1.0f, WHITE);
	}
};
class Player : public BaseModel
{
public:
	std::function<void(Vector3)> m_shootLaser;


	Player() = default;
	Player(Model model, Vector3 pos, std::function<void(Vector3)> shootLaser) : BaseModel(model,pos, {0,0,0}, PLAYER_SPEED), m_shootLaser(shootLaser)
	{

	}

	void input()
	{
		m_dir.x = (float)(IsKeyDown(KEY_D)) - (float)(IsKeyDown(KEY_A));
		//m_dir.z = (float)(IsKeyDown(KEY_S)) - (float)(IsKeyDown(KEY_W));

		if (m_dir.x != 0.0f || m_dir.z != 0.0f)
			m_dir = Vector3Normalize(m_dir);

		if (IsKeyPressed(KEY_SPACE))
		{
			m_shootLaser(m_pos);
		}
	}

	void update(float dt) override
	{
		input();
		move(dt);
		constraints();
	}

	void constraints()
	{
		m_pos.x = std::max(-6.f, std::min(m_pos.x, 7.f));

	}
};
class Laser : public BaseModel
{
public:
	Laser(Model model, Vector3 pos, Texture texture) : BaseModel(model, pos, Vector3{0,0,-1}, LASER_SPEED)
	{
		SetMaterialTexture(&model.materials[0], MATERIAL_MAP_ALBEDO, texture);
	}
};
class Meteor : public BaseModel
{
public:
	float radius = 1.0f;
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 rotationSpeed = { 0.0f, 0.0f, 0.0f };
	Shader shader;
	int flash_loc;
	Vector2 flashAmount;
	bool hit = false;
	//Timer deathTimer;
	float deathElapsed = 0.0f;

	Meteor() = default;

	Meteor(Texture2D texture)
	{
		// setup
		m_pos = {
			-6.0f + ((float)rand() / RAND_MAX) * 13.0f,   // [-6, 7]
			0.0f,
			-20.0f
		};

		radius = 0.6f + ((float)rand() / RAND_MAX) * (1.5f - 0.6f);

		m_model = LoadModelFromMesh(GenMeshSphere(radius, 8, 8));
		SetMaterialTexture(&m_model.materials[0], MATERIAL_MAP_ALBEDO, texture);

		m_speed = (float)(METEOR_SPEED_MIN + rand() % (METEOR_SPEED_MAX - METEOR_SPEED_MIN + 1));

		m_dir = {
			0.0f,
			0.0f,
			0.75f + ((float)rand() / RAND_MAX) * (1.25f - 0.75f)
		};

		// rotation
		rotation = {
			-5.0f + ((float)rand() / RAND_MAX) * 10.0f,
			-5.0f + ((float)rand() / RAND_MAX) * 10.0f,
			-5.0f + ((float)rand() / RAND_MAX) * 10.0f
		};

		rotationSpeed = {
			-1.0f + ((float)rand() / RAND_MAX) * 2.0f,
			-1.0f + ((float)rand() / RAND_MAX) * 2.0f,
			-1.0f + ((float)rand() / RAND_MAX) * 2.0f
		};

		// despawn logic
		//deathTimer = Timer(0.25f, false, false, [this]() { activeDespawn(); }); // timer doesnt auto start, only starts when called active

		shader = LoadShader(nullptr, "shaders/flash.fs");
		m_model.materials[0].shader = shader;
		flash_loc = GetShaderLocation(shader, "flash");
		flashAmount = { 1.f,0.f };
	}

	void flash() const
	{
		SetShaderValue(shader, flash_loc, &flashAmount, SHADER_UNIFORM_VEC2);
	}

	/*
		void activeDespawn()
	{
		std::cout << "activeDespawn called\n";

		m_despawn = true;
	}
	*/


	void update(float dt) override
	{
		//deathTimer.update();

		if (hit)
		{
			deathElapsed += dt;
			if (deathElapsed >= 0.25f)
				m_despawn = true;
		}

		if (!hit)
		{
			BaseModel::update(dt);

			rotation.x += rotationSpeed.x * dt;
			rotation.y += rotationSpeed.y * dt;
			rotation.z += rotationSpeed.z * dt;

			m_model.transform = MatrixRotateXYZ(rotation);
		}
	}

	void draw() override
	{
		DrawModel(m_model, m_pos, 1.0f, WHITE);
	}
};