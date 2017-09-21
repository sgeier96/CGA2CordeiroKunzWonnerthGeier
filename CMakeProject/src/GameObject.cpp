#include <GameObject.h>

GameObject::GameObject() {
	modelmat = glm::mat4(1.0f);
}

GameObject::~GameObject() {

}

void GameObject::render(double dtime) {
	
	GLint location;
	location = glGetUniformLocation(shaderProgram->prog, "model_mat");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(modelmat));

	//Die VAOs der einzelnen objects binden
	for (auto& eachVAO : objVAO)
	{
		eachVAO->bind();
		glDrawElements(GL_TRIANGLES, eachVAO->indexCount, GL_UNSIGNED_INT, 0);
		eachVAO->unbind();
	}
}

void GameObject::bindTexture(int textureUnit) {
	this->objTexture->bind(textureUnit);
}

void GameObject::unbindTexture() {
	this->objTexture->unbind();
}

void GameObject::bindHeightmap(int textureUnit) {
	this ->heightmap->bind(textureUnit);
}

void GameObject::rotate(float angle, int axis, GLdouble dtime) {
	switch (axis)
	{
	case 0:
		modelmat = modelmat * glm::rotate(angle * glm::pi<GLfloat>() * static_cast<GLfloat>(dtime), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 1:
		modelmat = modelmat * glm::rotate(angle * glm::pi<GLfloat>() * static_cast<GLfloat>(dtime), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 2: 
		modelmat = modelmat * glm::rotate(-angle * glm::pi<GLfloat>() * static_cast<GLfloat>(dtime), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 3:
		modelmat = modelmat * glm::rotate(-angle * glm::pi<GLfloat>() * static_cast<GLfloat>(dtime), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	default:
		throw "wrong axis. Choose between RIGHT, LEFT, UP and DOWN";
		break;
	}
}

void GameObject::translate(glm::vec3 transVec, GLdouble dtime) {
	modelmat =  glm::translate(modelmat, transVec * static_cast<GLfloat> (dtime));
}

void GameObject::scale(glm::vec3 scaleVec, GLdouble dtime) {
	modelmat = glm::scale(modelmat, scaleVec * static_cast<GLfloat>(dtime));
}

void GameObject::initiatoryRotation(float angle, int axis) {
	switch (axis)
	{
	case 0:
		modelmat = modelmat * glm::rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 1:
		modelmat = modelmat * glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		modelmat = modelmat * glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	default:
		throw "wrong axis. Choose between RIGHT, LEFT, UP and DOWN";
		break;
	}
}

void GameObject::initiatoryTranslation(glm::vec3 transVec) {
	modelmat = glm::translate(modelmat, transVec);
}

void GameObject::initiatoryScaling(glm::vec3 scaleVec) {
	modelmat = glm::scale(modelmat, scaleVec);
}


void GameObject::setTexturePreferences() {
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