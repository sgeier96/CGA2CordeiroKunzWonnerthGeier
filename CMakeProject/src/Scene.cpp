#include "Scene.h"


void Scene::init() {

#pragma region Asset loading
	assetManager.loadObject("../../assets/models/plane.obj"); //water surface
	assetManager.loadObject("../../assets/models/acropolis.obj");
	assetManager.loadObject("../../assets/models/plane.obj"); //ground/sand


	assetManager.loadTexture("../../assets/textures/waterSurface.png");
	assetManager.loadTexture("../../assets/textures/perlinNoiseTexture.png");
	assetManager.loadTexture("../../assets/textures/brickthingy.png");
	assetManager.loadTexture("../../assets/textures/GroundClay.png");

	assetManager.loadShaderProgram("../../assets/shaders/perlinshader.vert", "../../assets/shaders/perlinshader.frag");
	assetManager.loadShaderProgram("../../assets/shaders/simpleshader.vert", "../../assets/shaders/simpleshader.frag");
#pragma endregion

#pragma region texture assignment for each object
	assetManager.objects[0]->objTexture = assetManager.textures[0]; //water surface using waterSurface
	assetManager.objects[1]->objTexture = assetManager.textures[2]; //acropolis using brickthingy
	assetManager.objects[2]->objTexture = assetManager.textures[3]; //ground/sand using GroundClay

	assetManager.objects[0]->heightmap = assetManager.textures[1]; //plane using perlinNoiseTexture
#pragma endregion

#pragma region shaderProgramm assignment for each object
	assetManager.objects[0]->shaderProgram = assetManager.shaderPrograms[0]; //plane using perlinshader.vert/.frag
	assetManager.objects[1]->shaderProgram = assetManager.shaderPrograms[1]; //acropolis using simpleshader.vert/.frag
	assetManager.objects[2]->shaderProgram = assetManager.shaderPrograms[1]; //acropolis using simpleshader.vert/.frag
#pragma endregion


	lightDir = glm::vec3(-1.0f, -1.0f, -1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	projmat = glm::perspective(glm::radians(90.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

	initiatePerlinNoise(238);

#pragma region Settings and uniform uploads

	setSceneSettings();

	assetManager.objects[0]->shaderProgram->use();
	assetManager.objects[0]->bindHeightmap(0); GLERR
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "perlinTex"); GLERR
	glUniform1i(location, 0); GLERR


		for (auto& eachObject : assetManager.objects) {
			//Das oben kompilierte Shader Programm wird in die Pipeline gehängt,
			//(entspricht glUseProgram(program->prog);
			eachObject->shaderProgram->use();

			//Uniforms für den Vertex Shader
			location = glGetUniformLocation(eachObject->shaderProgram->prog, "proj_mat"); GLERR
			glUniformMatrix4fv(location, 1, false, glm::value_ptr(projmat)); GLERR
				//Uniforms für den Fragment Shader
			location = glGetUniformLocation(eachObject->shaderProgram->prog, "lightDir"); GLERR
			glm::vec4 lightDirInViewSpace(glm::normalize(viewmat * glm::vec4(lightDir, 0.0f)));
			//Vec3 daraus konstruieren und in die Uniform Variable schreiben
			glUniform3fv(location, 1, glm::value_ptr(glm::vec3(lightDirInViewSpace.x, lightDirInViewSpace.y, lightDirInViewSpace.z)));
			//Lichtfarbe
			location = glGetUniformLocation(eachObject->shaderProgram->prog, "light_color"); GLERR
			glUniform3fv(location, 1, glm::value_ptr(lightColor)); GLERR
		}
#pragma endregion

#pragma region placing objects in worldspace
	//acropolis
	assetManager.objects[1]->initiatoryScaling(glm::vec3(0.05f));
	assetManager.objects[1]->initiatoryTranslation(glm::vec3(30.0f, -47.0f, 0.0f));
	assetManager.objects[1]->initiatoryRotation(glm::radians(5.0f), 0);
	assetManager.objects[1]->initiatoryRotation(glm::radians(-2.0f), 2);

	//water surface
	assetManager.objects[0]->initiatoryRotation(glm::radians(180.0f), 0);

	//ground
	assetManager.objects[2]->initiatoryTranslation(glm::vec3(0.0f, 0.0f, -10.0f));
#pragma endregion

}

void Scene::render(double dtime) {

	/*Stattdessen man für jedes Objekt das jeweilige ShaderProgram anschließt, kann (und evtl. sollte man) eine List anlegen, 
		in der die einzelnen Objekte nach shaderProgram sortiert sind. Dann wird diese Liste durchlaufen und der o->shaderProgram->use()
		nur aufgerufen, sobald ein neuer "Objekttyp" erscheint, der einen anderen shaderProgram nutzt. Dadurch wird nicht mehr immer
		wieder der selbe shader neu an die pipeline gehänt, obwohl er schon darauf ist.*/
	/*Die feste Zuweisung von TexturUnits könnte noch verbessert werden. Vielleicht mit einer Art Zähler?*/
	for (auto& o : assetManager.objects) {
			o->shaderProgram->use();
			o->bindTexture(1); GLERR
				setTexturePreferences(); GLERR
				location = glGetUniformLocation(o->shaderProgram->prog, "tex"); GLERR
				glUniform1i(location, 1); GLERR

		o->render(dtime);
	}

	GLint location;
	//View Matrix
	viewmat = myCamera.updateViewmat();
	for (auto& eachObject : assetManager.objects) {
		eachObject->shaderProgram->use(); GLERR
		location = glGetUniformLocation(eachObject->shaderProgram->prog, "view_mat"); GLERR
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewmat)); GLERR
	}
	

}

void Scene::update(double dtime) {
	//passing u_time
	assetManager.objects[0]->shaderProgram->use(); GLERR
	u_time += dtime;
	location = glGetUniformLocation(assetManager.objects[0]->shaderProgram->prog, "u_time"); GLERR
	glUniform1f(location, u_time); GLERR
	//std::cout << "u_time: " <<  u_time << "\n";
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

void Scene::initiatePerlinNoise(int seed) {
	// Create an empty PPM image
	ppm image(800, 600);

	// Create a PerlinNoise object with a random permutation vector generated with seed
	PerlinNoise pn(seed);

	unsigned int kk = 0;
	// Visit every pixel of the image and assign a color generated with Perlin noise
	for (unsigned int i = 0; i < 600; ++i) {     // y
		for (unsigned int j = 0; j < 800; ++j) {  // x
			double x = (double)j / ((double)800);
			double y = (double)i / ((double)600);

			// Typical Perlin noise
			double n = pn.noise(10 * x, 10 * y, 0.8);


			// Map the values to the [0, 255] interval, for simplicity we use 
			// tones of grey
			image.r[kk] = floor(255 * n);
			image.g[kk] = floor(255 * n);
			image.b[kk] = floor(255 * n);
			kk++;
		}
	}

	// Save the image in a binary PPM file
	image.write("../../assets/textures/perlinNoiseTexture.ppm");
}

Scene::Scene(int windowX, int windowY) :
	windowWidth(windowX), windowHeight(windowY)
{
}

Scene::~Scene() {

}
