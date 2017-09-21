#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "GLUtils.h"
#include <libheaders.h>
#include <glerror.h>

class GameObject
{
public:
	GameObject();
	virtual~GameObject();

	virtual void render(double dtime); 

	int textureUnitCounter = 0;
	glm::vec3 previousPos;
	GLint location;


	//.bindTextures kann hier eingefügt werden

	//Das VAO für unsere Objekte
	std::vector<std::shared_ptr<VAO>> objVAO;
	//Das Shader Programm
	std::shared_ptr<ShaderProgram> shaderProgram;
	//Die Textur der Objekte
	std::shared_ptr<Texture2D> objTexture;
	//Optional: Heightmap der Objekte
	std::shared_ptr<Texture2D> heightmap;

	//Transformationsmatrix für die Objekte
	glm::mat4 modelmat;
	void bindTexture(int textureUnit);
	void unbindTexture();
	void bindHeightmap(int textureUnit);
	void unbindHeightmap(int textureUnit);
	void setTexturePreferences();

	void rotate(float, int, GLdouble);
	void translate(glm::vec3, GLdouble);
	void scale(glm::vec3, GLdouble);
	glm::vec3 translateIntoCenter();

	void initiatoryRotation(float angle, int axis);
	void initiatoryTranslation(glm::vec3 transVec);
	void initiatoryScaling(glm::vec3 scaleVec);
};

#endif