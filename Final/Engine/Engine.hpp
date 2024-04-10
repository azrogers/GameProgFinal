#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <debug_draw.hpp>

#include "OpenGL.hpp"
#include "SDL.hpp"
#include "Display.hpp"
#include "InputManager.hpp"
#include "Utility.hpp"
#include "ECS/Skybox.hpp"
#include "ECS/LambdaUpdater.hpp"
#include "Assets/AssimpLoader.hpp"
#include "Graphics/MeshPrimitives.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Light.hpp"
#include "StatusReporter.hpp"
#include "Audio/AudioContext.hpp"
#include "Audio/AudioClip.hpp"
#include "Audio/AudioListener.hpp"
#include "Audio/AudioSource.hpp"