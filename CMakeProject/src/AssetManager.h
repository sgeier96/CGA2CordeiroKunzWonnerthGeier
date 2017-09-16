#ifndef _ASSET_MANAGER_H_
#define _ASSET_MANAGER_H_

#include <OBJLoader.h>
#include <GameObject.h>
#include <string>


class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void loadObject(std::string path);
	void loadTexture(std::string path);
	void loadShaderProgram(std::string pathVertshader, std::string pathFragshader);

	 std::vector<std::unique_ptr<GameObject>> objects;
	std::vector<std::shared_ptr<Texture2D>> textures;
	std::vector<std::shared_ptr<ShaderProgram>> shaderPrograms;
};

#endif // !_ASSET_HANDLER_H_
