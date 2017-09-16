#include "Scene.h"




void Scene::init() {
	std::unique_ptr<GameObject> statue(new GameObject());

#pragma region Asset loading
	OBJResult res = OBJLoader::loadOBJ("../../assets/models/acropolis.obj");
	for each (auto eachObject in res.objects)
	{
		for each (auto eachMesh in eachObject.meshes)
		{
			statue->objVAO.push_back(GLUtils::createVAO(eachMesh));
		}
	}
	statue->objTexture = GLUtils::loadGLTexture("../../assets/textures/brickthingy.png");

	statue->shaderProgram = GLUtils::createShaderProgram("../../assets/shaders/simpleshader.vert", "../../assets/shaders/simpleshader.frag");
#pragma endregion

	lightDir = glm::vec3(-1.0f, -1.0f, -1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	projmat = glm::perspective(glm::radians(90.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);


#pragma region Einstellungen und Hochladen von Texturen,  etc
	//Wir aktivieren Backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Und den Tiefentest
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Das oben kompilierte Shader Programm wird in die Pipeline gehängt,
	//(entspricht glUseProgram(program->prog);
	statue->shaderProgram->use();

	//Wir setzen die Uniforms. Da sich die Daten in diesem einfachen Beispiel nicht ändern, können wir die Uniforms
	//hier in init setzen, die Werte werden im Shader Programm gespeichert.

	//Hilfsvariable für die Uniform Location
	GLint location;

	//Uniforms für den Vertex Shader


		//Projektions Matrix
		location = glGetUniformLocation(statue->shaderProgram->prog, "proj_mat"); GLERR
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(projmat)); GLERR



		//Uniforms für den Fragment Shader

		//Lichtrichtung
		location = glGetUniformLocation(statue->shaderProgram->prog, "lightDir"); GLERR
		//Lichtrichtung von Worldspace in Viewspace transformieren
		glm::vec4 lightDirInViewSpace(glm::normalize(viewmat * glm::vec4(lightDir, 0.0f)));
	//Vec3 daraus konstruieren und in die Uniform Variable schreiben
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(lightDirInViewSpace.x, lightDirInViewSpace.y, lightDirInViewSpace.z)));

	//Lichtfarbe
	location = glGetUniformLocation(statue->shaderProgram->prog, "light_color"); GLERR
		glUniform3fv(location, 1, glm::value_ptr(lightColor)); GLERR

	//Wir binden die Textur an die Texture Unit 0
		statue->objTexture->bind(0);

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
	statue->objTexture->unbind();

	location = glGetUniformLocation(statue->shaderProgram->prog, "tex"); GLERR
		glUniform1i(location, 0); GLERR
#pragma endregion
	objects.push_back(std::move(statue));
	//objects.push_back(std::unique_ptr<GameObject>(new Plane()));
}

void Scene::render(double dtime) {
	for (auto& o : objects) {
		o->render(dtime);
	}

	GLint location;
	//View Matrix
	viewmat = myCamera.updateViewmat();
	for each(auto& eachObject in objects) {
		location = glGetUniformLocation(eachObject->shaderProgram->prog, "view_mat"); GLERR
			glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewmat)); GLERR
	}
	
}

Scene::Scene(int windowX, int windowY) :
windowWidth(windowX), windowHeight(windowY)
{
}

Scene::~Scene() {

}

void Scene::moveCamera(int moveDirection, GLdouble dtime) {
	myCamera.move(moveDirection, dtime);
}

void Scene::passMouseMovement(float mouseXOffset, float mouseYOffset) {
	myCamera.processMouseMovement(mouseXOffset, mouseYOffset);
}

void Scene::rotateObject(int indice, float angle, int axis, GLdouble dtime) {
	objects[indice]->rotate(angle, axis, dtime);
}

void Scene::translateObject(int indice, glm::vec3 transVec, GLdouble dtime) {
	objects[indice]->translate(transVec, dtime);
}

void Scene::scaleObject(int indice, glm::vec3 scaleVec, GLdouble dtime) {
	objects[indice]->scale(scaleVec, dtime);
}

