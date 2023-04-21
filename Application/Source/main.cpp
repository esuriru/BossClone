#include "Core/Application.h"


int main()
{
	Application* app = Application::Instance();
	app->Run();
	app->Destroy();
}