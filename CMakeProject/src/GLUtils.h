#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include <../framework/libheaders.h>
#include <../libs/soil/include/SOIL.h>
#include <string>
#include <vector>
#include <../framework/OBJLoader.h>
#include <memory>

//Klassennamen hier vorw�rtsdeklariert, damit wir den Code dazu weiter unten einf�gen k�nnen.
class VAO;
class Texture2D;
class ShaderProgram;

//Die Hilfsklasse.
class GLUtils
{
public:
	//Die Klasse hat nur statische Funktionen und soll nicht instantiiert werden.
	//M�chte man auf Nummer Sicher gehen, l�scht man die Konstruktoren der Klasse, damit kein Objekt von ihr erzeugt werden kann.
	GLUtils() = delete;					//Standardkonstruktor l�schen
	GLUtils(const GLUtils&) = delete;	//Koperkonstruktor l�schen
	GLUtils(GLUtils&&) = delete;		//Movekonstruktor l�schen. (Die Move Semantik ist ein sehr wichtiges Feature
										//in C++11. Schaut euch f�r eure eigenen Datenstrukturen unbedingt Tutorials
										//dazu an.)
	
	//Die Hilfsmethoden

	/* Warum geben wir Shared Pointer zur�ck?
	Unsere Wrapper Klassen Texture2D, VAO und ShaderProgram geben bei Zerst�rung
	Die verpackten OpenGL Objekte wieder frei. Wenn wir einfach eine Kopie zur�ckgeben w�rden,
	w�rde das tempor�re Objekt in der Methode zerst�rt werden und damit auch das enthaltene
	OpenGL Objekt. Die Kopie des Objektes w�rde dann immer ung�ltige OpenGL Objekte
	enthalten, weil diese ja schon freigegeben wurden.

	Die einfachste M�glichkeit ist es, das Objekt der Wrapper Klasse einmal auf dem Heap zu erzeugen
	und einen Shared Pointer darauf zur�ckzugeben. Somit wird das OpenGL Objekt erst dann
	freigegeben, wenn die letzte Kopie des Shared Pointers zerst�rt wird. Genau
	das was wir wollen.
	*/

	//Textur aus Datei laden und ein OpenGL Texturobjekt erstellen.
	//Wir �bergeben den Pfad zur Texturdatei
	static std::shared_ptr<Texture2D> loadGLTexture(const std::string& path);

	//VAO aus einem OBJMesh erstellen. OBJMeshes bekommt man vom OBJLoader.
	static std::shared_ptr<VAO> createVAO(const OBJMesh& mesh);

	//Shader kompilieren und fertiges Shader Programm zur�ckgeben, das zum Rendern benutzt werden kann.
	//Wir �bergeben den Pfad zum Vertex- und Fragment Shader
	static std::shared_ptr<ShaderProgram> createShaderProgram(const std::string& vspath, const std::string& fspath);
};


//Einfache VAO Wrapper Klasse. Gibt das OpenGL VAO automatisch frei, wenn das Objekt dieser Klasse zerst�rt wird.
class VAO
{
public:
	//Konstruktor nimmt einen Vertexbuffer, einen Indexbuffer, ein Vertex Array Object und die Anzahl der zu zeichnenden Indices entgegen.
	VAO(GLuint vbo, GLuint ibo, GLuint vao, GLsizei indexct);
	~VAO();	//Im Destruktor werden vbo, ibo und vao wieder freigegeben.

	//Binden und unbinden des VAOs
	void bind();
	void unbind();

	GLuint vbo;	//OpenGL Vertex Buffer
	GLuint ibo; //OpenGL Index Buffer
	GLuint vao; //OpenGL Vertex Array Object
	GLsizei indexCount; //Anzahl der Indices die zu zeichnen sind
};

//Einfache 2D-Textur Wrapper Klasse
class Texture2D
{
public:
	//Dem Konstruktor �bergeben wir das OpenGL Textur-Objekt
	Texture2D(GLuint texture);
	//Der Destruktor gibt das Objekt frei wenn das Texture2D Objekt zerst�rt wird.
	~Texture2D();

	//Binden der Textur an eine Texture Unit. 0 ist ein Defaultwert. Wenn man keinen Parameter beim Aufruf angibt,
	//wird 0 angenommen.
	void bind(GLuint textureUnit = 0);
	//Unbinden der Textur
	void unbind();

	//OpenGL Textur Objekt
	GLuint tex;

	//Die Textureinheit, auf der die Textur als letztes gebunden wurde
	GLuint tu;
};

//Simple Wrapper Klasse f�r ein fertiges Shader Programm
class ShaderProgram
{
public:
	//Der Konstruktor bekommt das OpenGL Program Object
	ShaderProgram(GLuint program);
	//Der Destuktor gibt es wieder frei
	~ShaderProgram();

	//Use ruft glUseProgram(prog) auf.
	void use();

	//Das OpenGL Program Objekt
	GLuint prog;
};












#endif // !_GL_UTILS_H_
