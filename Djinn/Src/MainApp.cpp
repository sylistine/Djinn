#include "MainApp.h"



MainApp::MainApp(AppWrapper *appWrapper) : appWrapper(appWrapper)
{
    // Presently only using appWrapper here, but want to keep it for
    // later requesting new renderers.
    renderer = appWrapper->GetRenderer();
}


MainApp::~MainApp()
{
}


void MainApp::Update()
{
    GameUpdate();
    renderer->Draw();
}


void MainApp::GameUpdate()
{
    
}
