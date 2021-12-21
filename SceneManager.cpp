
#include "scene.h"

//**********************************************
//
//		シーンの管理
//
//**********************************************

void SceneManager::Finalize()
{
    currentScene->Finalize();
}

void SceneManager::Update(float elapsedTime)
{
	currentScene->Update(elapsedTime);
}

void SceneManager::Render()
{
	currentScene->Render();
}

void SceneManager::ChangeScene(Scene* new_scene) {
    currentScene.reset(new_scene);
    // 2 重初期化されないようにする。  
    if (!currentScene->initialized) {
        currentScene->Initialize();
        currentScene->initialized = true;
    }
}
