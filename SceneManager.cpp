
#include "scene.h"

//**********************************************
//
//		�V�[���̊Ǘ�
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
    // 2 �d����������Ȃ��悤�ɂ���B  
    if (!currentScene->initialized) {
        currentScene->Initialize();
        currentScene->initialized = true;
    }
}
