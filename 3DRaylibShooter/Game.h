#pragma once
#include "raylib.h"
#include <vector>
#include "Model.h"
#include "Timer.h"

struct ModelAssets
{
	Model player{};
	Model laser{};
};

struct AudioAssets
{
	Sound laser{};
	Sound explosion{};
	Music music{};
};

class Game
{
public:
	Game();
	~Game();
	void run();
	void draw();
	void update();
	bool isGameOver = false;

	

private:

	void importAssets();
	void shootLaser(Vector3 pos);
	void spawnMeteor();
	void drawShadows() const;
	void CheckCollisions();
	void DespawnModels();
	void DrawTime() const;
	void DrawScore() const;
	void DespawnFarLaser();
	void DespawnFarMeteor();

	int score = 0;

	ModelAssets models;
	AudioAssets audio;

	std::vector<Texture2D> textures;
	Texture2D darkTexture{};
	Texture2D lightTexture{};
	Font font{};


	Camera3D camera;

	Player player;
	Floor floor;
	std::vector<Meteor> meteors;
	std::vector<Laser> lasers;
	Timer meteorTimer;
};

