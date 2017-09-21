#include "Game.h"

/*
- Erstmal den ganzen globalen shit binden (Lichtquellen in der Szene, View und Projektionsmatrix(Kamerakram)und OpenGL states setzen(ob ich tiefentest brauch, blending brauch(also alles was auf die komplette Szene angewandt wird)
- Die ganzen objekte durchgehen die ich rendern möchte und stelle alle Bedingungen fest die ich für das Rendern des Objektes brauche (Texturen, Matritzen, ...)
- 
*/

Game::Game() :
	GameWindow(800,				//width
		600,				//height
		false,				//fullscreen
		true,				//vsync
		3,					//OpenGL Version Major
		3,					//OpenGL Version Minor	=> Here the OpenGL Version is 3.3
		"Underwater scene"), //Tile of the window
	myScene(800,600)
{
}

Game::~Game()
{

}

//Initialization here. (i.e. load a scene, load textures ...)
void Game::init()
{
	myScene.init();
	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
}

//cleanup. Free resources here.
void Game::shutdown()
{
	//Da wir für VAO, Texture2D und ShaderProgram Wrapper Klassen haben,
	//die diese Objekte automatisch freigeben, müssen wir hier nichts tun.
}

//Update Game Logic here
void Game::update(GLdouble dtime)
{
#pragma region camera movement input
	if (input->getKeyState(Key::W) == KeyState::Pressed)
		myScene.moveCamera(MOVE_FORWARD, dtime);
	if (input->getKeyState(Key::S) == KeyState::Pressed)
		myScene.moveCamera(MOVE_BACKWARDS, dtime);
	if (input->getKeyState(Key::A) == KeyState::Pressed)
		myScene.moveCamera(MOVE_LEFT, dtime);
	if (input->getKeyState(Key::D) == KeyState::Pressed)
		myScene.moveCamera(MOVE_RIGHT, dtime);
#pragma endregion
	myScene.update(dtime);

#pragma region rotation commands
	if (input->getKeyState(Key::Right) == KeyState::Pressed)
		myScene.rotateObject(1, 0.5f, RIGHT, dtime);
	if (input->getKeyState(Key::Left) == KeyState::Pressed)
		myScene.rotateObject(1, 0.5f, LEFT, dtime);
	if (input->getKeyState(Key::Up) == KeyState::Pressed)
		myScene.rotateObject(1, 0.5f, UP, dtime);
	if (input->getKeyState(Key::Down) == KeyState::Pressed)
		myScene.rotateObject(1, 0.5f, DOWN, dtime);
	
#pragma endregion
#pragma region translation commands (CURRENTLY DEACTIVATED)
/*	if (input->getKeyState(Key::W) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(0.0f, 1.0f, 0.0f), dtime);
	if (input->getKeyState(Key::A) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(-1.0f, 0.0f, 0.0f), dtime);
	if (input->getKeyState(Key::S) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(0.0f, -1.0f, 0.0f), dtime);
	if (input->getKeyState(Key::D) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(1.0f, 0.0f, 0.0f), dtime);
	if (input->getKeyState(Key::Q) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(0.0f, 0.0f, -1.0f), dtime);
	if (input->getKeyState(Key::E) == KeyState::Pressed)
		myScene.translateObject(0, glm::vec3(0.0f, 0.0f, 1.0f), dtime);
*/
#pragma endregion
#pragma region scale commands
	if (input->getKeyState(Key::NumPadSubtract) == KeyState::Pressed)
		myScene.scaleObject(0, glm::vec3(0.2f, 0.2f, 0.2f), dtime);
	if (input->getKeyState(Key::NumPadAdd) == KeyState::Pressed)
		myScene.scaleObject(0, glm::vec3(1.2f, 1.2f, 1.2f), dtime);
#pragma endregion
}

//Render a frame
void Game::render(GLdouble dtime)
{
	// Colorbuffer und Depthbuffer des Backbuffers clearen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myScene.render(dtime);
}

//Keyboard events
void Game::onKey(Key key, Action action, Modifier modifier)
{
	if (key == Key::Escape && action == Action::Down)
		quit();
}

//Mouse move events
void Game::onMouseMove(MousePosition mouseposition)
{
	if (firstMouse)
	{
		mouseposition.oldX = mouseposition.X;
		mouseposition.oldY = mouseposition.Y;
		firstMouse = false;
	}
		mouseXOffset =static_cast<float>( mouseposition.X - mouseposition.oldX);
		mouseYOffset = static_cast<float>(mouseposition.oldY - mouseposition.Y);

		mouseposition.oldX = mouseposition.X;
		mouseposition.oldY = mouseposition.Y;
		myScene.passMouseMovement(mouseXOffset, mouseYOffset);
}

//Mouse Button events
void Game::onMouseButton(MouseButton button, Action action, Modifier modifier)
{

}

//Mouse scroll events
void Game::onMouseScroll(double xscroll, double yscroll)
{

}

//Window resize events
void Game::onFrameBufferResize(int width, int height)
{
	glViewport(0, 0, width, height);
}
