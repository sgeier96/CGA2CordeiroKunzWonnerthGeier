#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "GLUtils.h"
#include <libheaders.h>

class GameObject
{
public:
	GameObject();
	virtual~GameObject();

	virtual void render(double dtime); 
	/* "= 0 bewirkt, dass render in der Basisklasse keine Implementierung bekommt
		d.h. Es können nur Objekte von Klassen erzeugt werden die von dieser Klasse erben
		ist eine Funktion einer Klasse als = 0 deklariert nennt sich die Klasse "abstrakt"" */


	//.bindTextures kann hier eingefügt werden

	//Das VAO für unsere Objekte
	std::vector<std::shared_ptr<VAO>> objVAO;
	//Das Shader Programm
	std::shared_ptr<ShaderProgram> shaderProgram;
	//Die Textur der Objekte
	std::shared_ptr<Texture2D> objTexture;

	//Transformationsmatrix für die Objekte
	glm::mat4 modelmat;


	void rotate(float, int, GLdouble);
	void translate(glm::vec3, GLdouble);
	void scale(glm::vec3, GLdouble);
};

#endif