#include "Game.h"
#include "Settings.h"

Game::Game()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shooter Game 3D");
    InitAudioDevice();
    importAssets();

    //camera
    camera.position = Vector3{ -4.0f, 8.0f, 6.0f };
    camera.target = Vector3{ 0.0f,0.0f,-1.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    player = Player(models.player, {0,0,0}, [this](Vector3 pos) { shootLaser(pos); });


    floor = Floor(darkTexture);

    meteorTimer = Timer(
        METEOR_TIMER_DURATION, true, true,
        [this]() { spawnMeteor(); }
    );

    PlayMusicStream(audio.music);
    SetMasterVolume(0.2);
}
Game::~Game()
{
    UnloadModel(models.laser);
    UnloadModel(models.player);

    for (auto& t : textures)
    {
        UnloadTexture(t);
    }
    UnloadTexture(darkTexture);
    UnloadTexture(lightTexture);

    UnloadFont(font);
    UnloadSound(audio.explosion);
    UnloadSound(audio.laser);
    UnloadMusicStream(audio.music);
    CloseAudioDevice();
    CloseWindow();
}
void Game::run()
{
	
	while (!WindowShouldClose() && !isGameOver)
	{
        update();
		draw();
	}

}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    floor.draw();
    player.draw();
    drawShadows();
    for (auto& m : meteors) m.draw();
    for (auto& l : lasers) l.draw();
    EndMode3D();

    DrawTime();
    DrawScore();
    EndDrawing();
}

void Game::CheckCollisions()
{
    for (auto& l : lasers)
    {
        for (auto& m : meteors)
        {
            BoundingBox laserBBox = GetMeshBoundingBox(l.m_model.meshes[0]); // bounding box does not move by default
            BoundingBox worldPosLaserBBox{}; // repositions bounding box to laser's position

            worldPosLaserBBox.min = Vector3Add(laserBBox.min, l.m_pos);
            worldPosLaserBBox.max = Vector3Add(laserBBox.max, l.m_pos);

            bool collided = CheckCollisionBoxSphere(worldPosLaserBBox, m.m_pos, m.radius);

            if (collided && !m.hit)
            {
                m.hit = true;
                //m.hitTime = GetTime();
                l.m_despawn = true;
                //m.deathTimer.activate();
                //m.m_despawn = true;
                m.flash();
                ++score;

                PlaySound(audio.explosion);
            }

        }
    }
    for (auto& m : meteors)
    {
        bool collided = CheckCollisionSpheres(player.m_pos, 0.8, m.m_pos, m.radius );
        if (collided)
        {
            isGameOver = true;
        }
    }


}

void Game::DespawnModels()
{
    for (auto l = lasers.begin(); l != lasers.end();)
    {
        if (l->m_despawn)
        {
            l = lasers.erase(l);
        }
        else
        {
            ++l;
        }
    }
      
    for (auto m = meteors.begin(); m != meteors.end();)
    {
        if (m->m_despawn)
        {
            m = meteors.erase(m);
        }
        else
        {
            ++m;
        }
    }
}

void Game::DrawTime() const
{
    int time = (int)GetTime();
    std::string timetext = "Time: " + std::to_string(time);

    DrawTextEx(font, timetext.c_str(), { (WINDOW_WIDTH - 200 - FONT_PADDING), WINDOW_HEIGHT - FONT_PADDING}, FONT_SIZE, 2, WHITE);
}

void Game::DrawScore() const
{
    std::string scorestr = "Score: " + std::to_string(score);
    DrawTextEx(font, scorestr.c_str(), { (WINDOW_WIDTH - 500 - FONT_PADDING), WINDOW_HEIGHT - FONT_PADDING }, FONT_SIZE, 2, WHITE);
}

void Game::DespawnFarLaser()
{
    for (auto& l : lasers)
    {
        if (l.m_pos.z < -20)
        {
            l.m_despawn = true;
            std::cout << "laser far despawn \n";
        }
    }
}
void Game::DespawnFarMeteor()
{
    for (auto& m : meteors)
    {
        if (m.m_pos.z > 20.0f)
        {
            m.m_despawn = true;
            std::cout << "meteor far despawn \n";

        }
    }
}

void Game::shootLaser(Vector3 pos)
{
    lasers.push_back(Laser(models.laser, pos, lightTexture));
    PlaySound(audio.laser);
    //std::cout << "shoot laser" << '\n';
}
void Game::update()
{
	float dt = GetFrameTime();
    meteorTimer.update();
    player.update(dt);
    for (auto& m : meteors) m.update(dt);
    for (auto& l : lasers) l.update(dt);

    
    CheckCollisions();
    DespawnFarLaser();
    DespawnFarMeteor();
    DespawnModels();
    UpdateMusicStream(audio.music);

}
void Game::spawnMeteor()
{
    int texIndex = rand() % textures.size();
    meteors.push_back(Meteor(textures[texIndex]));
}
void Game::drawShadows() const
{
    float playerRadius = 0.5 + player.m_pos.y;
    DrawCylinder(Vector3{ player.m_pos.x, -1.5, player.m_pos.z }, playerRadius, playerRadius, 0.1, 20, { 0,0,0,50 });

    for (auto& m : meteors)
    {
        
        DrawCylinder(Vector3{m.m_pos.x, -1.5, m.m_pos.z}, m.radius, m.radius, 0.1, 20, { 0,0,0,50 });
    }
}
void Game::importAssets()
{
    models.player = LoadModel("models/ship.glb");
    models.laser = LoadModel("models/laser.glb");

    audio.laser = LoadSound("audio/laser.wav");
    audio.explosion = LoadSound("audio/explosion.wav");
    audio.music = LoadMusicStream("audio/music.wav");

    textures.push_back(LoadTexture("textures/red.png"));
    textures.push_back(LoadTexture("textures/green.png"));
    textures.push_back(LoadTexture("textures/orange.png"));
    textures.push_back(LoadTexture("textures/purple.png"));

    darkTexture = LoadTexture("textures/dark.png");
    lightTexture = LoadTexture("textures/light.png");

    font = LoadFontEx("font/Stormfaze.otf", FONT_SIZE, nullptr, 0);
}

