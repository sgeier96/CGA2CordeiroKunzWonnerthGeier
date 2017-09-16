#include "Scene.h"


void Scene::init() {

#pragma region Asset loading
	assetManager.loadObject("../../assets/models/acropolis.obj");
	assetManager.loadObject("../../assets/models/male_head.obj");

	assetManager.loadTexture("../../assets/textures/brickthingy.png");
	//assetManager.loadTexture("../../assets/textures/CliffJagged004_COL_VAR2_1K.png");
	
	assetManager.loadShaderProgram("../../assets/shaders/simpleshader.vert", "../../assets/shaders/simpleshader.frag");
#pragma endregion
	
#pragma region texture assignment for each object
	assetManager.objects[0]->objTexture = assetManager.textures[0]; //acropolis using brickthingy
	assetManager.objects[1]->objTexture = assetManager.textures[0]; //head using brickthingy
#pragma endregion

#pragma region shaderProgramm assignment for each object
	assetManager.objects[0]->shaderProgram = assetManager.shaderPrograms[0]; //acropolis using simpleshader.vert/.frag
	assetManager.objects[1]->shaderProgram = assetManager.shaderPrograms[0]; //head using simpleshader.vert/.frag
#pragma endregion


	lightDir = glm::vec3(-1.0f, -1.0f, -1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	projmat = glm::perspective(glm::radians(90.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);


#pragma region Settings and uniform uploads
	setSceneSettings();
	GLint location;

	//Das oben kompilierte Shader Programm wird in die Pipeline gehängt,
	//(entspricht glUseProgram(program->prog);
	assetManager.objects[0]->shaderProgram->use();
	assetManager.objects[1]->shaderProgram->use();

	//Uniforms für den Vertex Shader
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "proj_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(projmat)); GLERR
	//Uniforms für den Fragment Shader
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "lightDir"); GLERR
	glm::vec4 lightDirInViewSpace(glm::normalize(viewmat * glm::vec4(lightDir, 0.0f)));
	//Vec3 daraus konstruieren und in die Uniform Variable schreiben
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(lightDirInViewSpace.x, lightDirInViewSpace.y, lightDirInViewSpace.z)));
	//Lichtfarbe
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "light_color"); GLERR
	glUniform3fv(location, 1, glm::value_ptr(lightColor)); GLERR

	//Wir binden die Textur an die Texture Unit 0
	assetManager.objects[0]->objTexture->bind(0);
	setTexturePreferences();
	assetManager.objects[0]->objTexture->unbind();
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "tex"); GLERR
	glUniform1i(location, 0); GLERR

	assetManager.objects[1]->objTexture->bind(1);
	setTexturePreferences();
	assetManager.objects[1]->objTexture->unbind();

#pragma endregion
}

void Scene::render(double dtime) {
	for (auto& o : assetManager.objects) {
		o->render(dtime);
	}

	GLint location;
	//View Matrix
	viewmat = myCamera.updateViewmat();
	for each(auto& eachObject in assetManager.objects) {
		location = glGetUniformLocation(eachObject->shaderProgram->prog, "view_mat"); GLERR
			glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewmat)); GLERR
	}
	
}

void Scene::moveCamera(int moveDirection, GLdouble dtime) {
	myCamera.move(moveDirection, dtime);
}

void Scene::passMouseMovement(float mouseXOffset, float mouseYOffset) {
	myCamera.processMouseMovement(mouseXOffset, mouseYOffset);
}

void Scene::rotateObject(int indice, float angle, int axis, GLdouble dtime) {
	assetManager.objects[indice]->rotate(angle, axis, dtime);
}

void Scene::translateObject(int indice, glm::vec3 transVec, GLdouble dtime) {
	assetManager.objects[indice]->translate(transVec, dtime);
}

void Scene::scaleObject(int indice, glm::vec3 scaleVec, GLdouble dtime) {
	assetManager.objects[indice]->scale(scaleVec, dtime);
}

void Scene::setSceneSettings() {
	//Wir aktivieren Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Und den Tiefentest
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Scene::setTexturePreferences() {
	//Wir setzen die Wrapmodes. (Was passiert wenn ich über die Textur hinaus sample?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GLERR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GLERR

		//Wir setzen die Textur Filter Einstellungen
		//Beim min Filter verwenden wir Mipmaps und interpolieren linear zwischen ihnen, damit man keine Übergänge in der Textur sieht.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); GLERR
		//Beim mag Filter verwenden wir lineare Interpolation
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLERR

		//Optional: Wir aktivieren ansiotrope Filterung. Das bewirkt, dass die Texturen aus einem flachen Blickwinkel
		//nicht matschig wirken.
		GLfloat aniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);	//Fragt die maximal mögliche Samplezahl ab
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); //Setzt die maximale Samplezahl.
																		  //Der Grafiktreiber entscheidet dann automatisch, wie viele Samples in einer bestimmten Situation
																		  //angebracht sind, kann aber maximal aniso Samples verwenden

																		  //Wir verbinden die Texture Unit, auf der unsere Textur liegt (0) mit dem Sampler im Shader.
																		  //Dazu setzen wir die Uniform Variable einfach auf die Nummer der Texture Unit also 0.
}

void Scene::uploadUniform(GLuint shaderProgram, const GLchar* uniformName) {
	
}

Scene::Scene(int windowX, int windowY) :
	windowWidth(windowX), windowHeight(windowY)
{
}

Scene::~Scene() {

}
