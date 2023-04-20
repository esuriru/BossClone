#include "Core/Application.h"


int main()
{
	Application* app = Application::GetInstance();
	app->Init();
	app->Run();
	app->Exit();
	app->Destroy();
}