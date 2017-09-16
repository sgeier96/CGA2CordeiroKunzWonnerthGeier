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

	//Das VAO der Statue binden
	for each (auto eachVAO in objVAO)
	{
		eachVAO->bind();
		glDrawElements(GL_TRIANGLES, eachVAO->indexCount, GL_UNSIGNED_INT, 0);
		eachVAO->unbind();
	}
	// Wir binden die Textur an die Texture Unit 0
	objTexture->bind(0);
					  
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
