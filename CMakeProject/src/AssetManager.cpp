#include "AssetManager.h"

AssetManager::AssetManager() {

}

AssetManager::~AssetManager() {

}

void AssetManager::loadObject(std::string path) {

	std::unique_ptr<GameObject> tmpObject(new GameObject());
	OBJResult res = OBJLoader::loadOBJ(path);
	for each (auto eachObject in res.objects)
	{
		for each (auto eachMesh in eachObject.meshes)
		{
			tmpObject->objVAO.push_back(GLUtils::createVAO(eachMesh));
		}
	}
	objects.push_back(std::move(tmpObject));
}

void AssetManager::loadTexture(std::string path) {
	textures.push_back(GLUtils::loadGLTexture(path));
}

void AssetManager::loadShaderProgram(std::string pathVertshader, std::string pathFragshader) {
	shaderPrograms.push_back(GLUtils::createShaderProgram(pathVertshader, pathFragshader));
}