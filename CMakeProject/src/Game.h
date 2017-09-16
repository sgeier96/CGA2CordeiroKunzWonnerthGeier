#ifndef _GAME_H_
#define _GAME_H_

#include <GameWindow.h>
#include <libheaders.h>
#include "OBJLoader.h"
#include "Scene.h"
#include "GameObject.h"

class Game : public GameWindow
{
public:
	Game();
	~Game();

	void init() override;
	void shutdown() override;

	void update(GLdouble dtime) override; 
	void render(GLdouble dtime) override;

	void onKey(Key key, Action action, Modifier modifier) override;
	void onMouseMove(MousePosition mouseposition) override;
	void onMouseButton(MouseButton button, Action action, Modifier modifier) override;
	void onMouseScroll(double xscroll, double yscroll) override;

	void onFrameBufferResize(int width, int height) override;

	enum Direction {
		RIGHT = 0,
		DOWN = 1,
		LEFT = 2,
		UP = 3,
		MAX_TYPES
	};

	enum moveDirection {
		MOVE_FORWARD = 0,
		MOVE_BACKWARDS = 1,
		MOVE_LEFT = 2,
		MOVE_RIGHT = 3,
		MOVE_MAX_TYPES
	};

private:

	Scene myScene;
	MousePosition myMousePosition;
	bool firstMouse = true;
	float mouseXOffset;
	float mouseYOffset;

	//Richtung des direktionalen Lichts
	glm::vec3 lightDir;
	//Farbe des Lichts
	glm::vec3 lightColor;

	//Viewmatrix
	glm::mat4 viewmat;
	//Projektionsmatrix
	glm::mat4 projmat;
	
};

#endif
