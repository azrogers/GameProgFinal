#include "Project.hpp"
#include "CarController.hpp"
#include "LightFlicker.hpp"

void CreateStreetLampSpot(eg::Object* node)
{
	eg::Light* light = eg::Light::CreateSpotLight(
		vec3(0.91, 1.0, 0.127),
		vec3(0.1, 0.1, 0.1),
		1.0f,
		45
	);

	eg::Light* pointLight = eg::Light::CreatePointLight(
		vec3(0.91, 1.0, 0.127),
		vec3(0.1, 0.1, 0.1),
		5.0f,
		0.0f 
	);

	node->AddComponent(light);
	node->AddComponent(pointLight);
	game::LightFlicker* flicker = new game::LightFlicker(light);
	flicker->lights.push_back(pointLight);
	node->AddComponent(flicker);
	node->transform.rotation = glm::quat(glm::vec3(glm::radians(-90.0f), 0, 0));
}

Project::Project(unsigned int width, unsigned int height) :
	display(width, height),
	phong(
		eg::Shader(GL_VERTEX_SHADER, utility::from_file("shaders/phong_vertex.glsl")),
		eg::Shader(GL_FRAGMENT_SHADER, utility::from_file("shaders/phong_fragment.glsl")),
		eg::Vertex::Array::Bindings()),
	skyboxShader(
		eg::Shader(GL_VERTEX_SHADER, utility::from_file("shaders/skybox_vertex.glsl")),
		eg::Shader(GL_FRAGMENT_SHADER, utility::from_file("shaders/skybox_fragment.glsl")),
		{ { "pos", 0 } }
	)
{
	input = display.InputManager();
	renderer = display.Renderer();
	debugConfig = new game::DebugConfig(input);

	scene = new eg::Object();
	scene->AddComponent(debugConfig);

	eg::Texture* skyboxTexture = new eg::Texture(
		"resource/skyboxes/DarkStormy/DarkStormyLeft2048.png",
		"resource/skyboxes/DarkStormy/DarkStormyRight2048.png",
		"resource/skyboxes/DarkStormy/DarkStormyUp2048.png",
		"resource/skyboxes/DarkStormy/DarkStormyDown2048.png",
		"resource/skyboxes/DarkStormy/DarkStormyFront2048.png",
		"resource/skyboxes/DarkStormy/DarkStormyBack2048.png"
	);

	eg::Skybox* skybox = new eg::Skybox(skyboxTexture, &skyboxShader);
	scene->AddComponent(skybox);

	eg::Object* worldMdl = eg::AssimpLoader::Load("resource/scene/final.dae", &phong);
	scene->Add(worldMdl);

	player = new game::Player(input);
	playerObj = worldMdl->FindFirstWithName("PlayerSpawn");
	playerObj->name = "Player";
	playerObj->AddComponent(player);
	playerObj->AddComponent(new eg::AudioListener());

	eg::AudioClip *ambience = new eg::AudioClip("resource/scene/ambience.ogg");
	eg::AudioSource *ambienceSource = new eg::AudioSource();
	ambience->Looping(true);
	ambienceSource->Clip(ambience);
	playerObj->AddComponent(ambienceSource);

	gui = new game::GUI(&display);
	gui->AddElement(debugConfig);
	gui->AddElement(player);
	scene->AddComponent(gui);

	game::CarController *car = new game::CarController();
	car->flWheel = worldMdl->FindFirstWithName("_7_Tire_FL");
	car->frWheel = worldMdl->FindFirstWithName("_7_Tire_FR");
	car->blWheel = worldMdl->FindFirstWithName("_7_Tire_BL");
	car->brWheel = worldMdl->FindFirstWithName("_7_Tire_BR");
	car->carSpeed = 10.0f;
	car->wheelSpeed = 10.0f;
	eg::Object* car1Obj = worldMdl->FindFirstWithName("_7Camaro");
	car1Obj->AddComponent(car);

	game::CarController *car2 = new game::CarController();
	car2->flWheel = worldMdl->FindFirstWithName("_7_Tire_FL_001");
	car2->frWheel = worldMdl->FindFirstWithName("_7_Tire_FR_001");
	car2->blWheel = worldMdl->FindFirstWithName("_7_Tire_BL_001");
	car2->brWheel = worldMdl->FindFirstWithName("_7_Tire_BR_001");
	car2->carSpeed = 10.0f;
	car2->wheelSpeed = 10.0f;
	car2->direction = -1.0f;
	eg::Object* car2Obj = worldMdl->FindFirstWithName("_7Camaro_001");
	car2Obj->AddComponent(car2);

	eg::AudioClip *engineClip = new eg::AudioClip("resource/scene/engine.wav");
	eg::AudioClip *engineClip2 = new eg::AudioClip("resource/scene/engine.wav");
	engineClip->Looping(true);
	engineClip2->Looping(true);
	eg::AudioSource *engineSource1 = new eg::AudioSource();
	engineSource1->Clip(engineClip);
	engineSource1->gain = 0.25f;
	car1Obj->AddComponent(engineSource1);
	eg::AudioSource *engineSource2 = new eg::AudioSource();
	engineSource2->Clip(engineClip2);
	engineSource2->gain = 0.25f;
	car2Obj->AddComponent(engineSource2);

	CreateStreetLampSpot(worldMdl->FindFirstWithName("StreetLampSpot"));
	CreateStreetLampSpot(worldMdl->FindFirstWithName("StreetLampSpot_001"));
	CreateStreetLampSpot(worldMdl->FindFirstWithName("StreetLampSpot_002"));
	CreateStreetLampSpot(worldMdl->FindFirstWithName("StreetLampSpot_003"));
	CreateStreetLampSpot(worldMdl->FindFirstWithName("StreetLampSpot_004"));

	player->playerMin = vec3(-50, -40, -60);
	player->playerMax = vec3(50, 0, 10);

	eg::Object* sunObj = new eg::Object();
	sunObj->transform.Rotate(-45, vec3(1, 0, 0));
	sunObj->transform.Translate(vec3(0, 10, 0));
	scene->Add(sunObj);

	renderer->fogColor = vec3(0.5, 0.5, 0.5);
	renderer->fogDensity = 0.05f;

	ambienceSource->Play();
	engineSource1->Play();
	engineSource2->Play();
}

void Project::Render(void)
{
	gui->PreDraw();

	debugConfig->ApplyConfig(&renderer->settings);

	renderer->Begin(player->GetCameraMatrix(), playerObj->transform.position);
	renderer->Render(scene);
	renderer->End();
}

void Project::ApplyTime(float seconds)
{
	scene->Update(seconds);
}

int Project::Run(void)
{
	Uint32 lastCall = 0;
	while(display.IsRunning())
	{
		display.PollEvents();
		Uint32 time = SDL_GetTicks();
		ApplyTime(static_cast<float>(time - lastCall) / 1000.0f);
		lastCall = time;
		Render();
	}

	return 0;
}