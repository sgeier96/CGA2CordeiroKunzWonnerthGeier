#ifndef _SCENE_H_
#define _SCENE_H_

#include <libheaders.h>
#include "GameObject.h"
#include "Camera.h"
#include <GameWindow.h>
#include <OBJLoader.h>
#include "GLUtils.h"
#include <list>
#include <memory>


/*ALS ERWEITERUNGSMÖGLICHKEIT KANN MAN IN EINER TEXTDATEI DIE PFADE ETC ANGEBEN */

class Scene
{
public:
	Scene(int, int);
	~Scene();

	void init();
	void render(double dtime);
	void rotateObject(int indice, float angle, int axis, GLdouble dtime);
	void translateObject(int indice, glm::vec3 transVec, GLdouble dtime);
	void scaleObject(int indice, glm::vec3 scaleVec, GLdouble dtime);

	void moveCamera(int moveDirection, GLdouble dtime);
	void passMouseMovement(float mouseXOffset, float mouseYOffset);
	

private:
	/*unique_ptr ist ein smart pointer der automatisch das objekt löscht wenn es out-of-scope geht.
	damit werden die objekte automatisch "aufgeräumt" wenn die Scene Klasse zerstört wird.*/
	std::vector<std::unique_ptr<GameObject>> objects;

	int windowWidth, windowHeight;
	Camera myCamera;

	//Richtung des direktionalen Lichts
	glm::vec3 lightDir;
	//Farbe des Lichts
	glm::vec3 lightColor;

	//Viewmatrix
	glm::mat4 viewmat;
	//Projektionsmatrix
	glm::mat4 projmat;

};
#endif // !_SCENE_H_