#include "GLUtils.h"
#include <glerror.h>

//GLUtils Definitionen

//Laden einer OpenGL Textur
std::shared_ptr<Texture2D> GLUtils::loadGLTexture(const std::string& path)
{
	//Variablen für SOIL. Hier schreibt SOIL Informationen zur Bilddatei rein.
	GLsizei width;	//Breite
	GLsizei height;	//Höhe
	GLsizei channels; //Anzahl der Farbkanäle. Bei RGBA z.B. 4
	
	//Rohdaten aus der Bilddatei laden. Wir übergeben den Dateipfad, die Adressen der Infovariablen von oben damit
	//SOIL dort hineinschreiben kann, und geben an dass SOIL eine RGBA Textur laden soll.
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	//image ist jetzt ein Zeiger auf die Rohdaten.
	
	GLuint texid = 0;  //OpenGL Texture Handle

	//Wenn image nullpointer ist, ist irgendetwas schief gegangen.
	if (image == nullptr)
	{ 
		//Wir werfen eine Exception. Darum soll sich der aufrufende Code kümmern.
		throw std::logic_error("Texture file coudn't be read.");
	}
	else
	{
		//Leeres OpenGL Texturobjekt erzeugen
		glGenTextures(1, &texid); GLERR	//create texture object

		if (texid == 0)
		{
			//Wenn das Handle 0 ist konnte das Objekt nicht erzeugt werden.
			throw std::logic_error("OpenGL texture object creation failed.");
		}

		//Andernfalls binden wir das Texturobjekt.
		glBindTexture(GL_TEXTURE_2D, texid); GLERR	//bind the texture object

		//Und laden die Rohdaten aus image hoch:
		glTexImage2D(
			GL_TEXTURE_2D,		//Das Target, an das die Daten übergeben werden sollen. Unsere Textur ist bereits an GL_TEXTURE_2D gebunden!
			0,					//Welches Mipmap level der Textur soll gefüllt werden? Natürlich das erste also 0. Mipmaps erzeugen wir später
			GL_RGBA8,			//Welches interne Format soll die Textur haben? Wir währen RGBA8, also RGBA mit jeweils 8 bit Breite für die Kanäle
			width,				//Breite des Bildes
			height,				//Höhe des Bildes
			0,					//Border parameter. Der ist veraltet und muss stets auf 0 gesetzt werden.
			GL_RGBA,			//Wie viele Komponenten hat die Textur? Wir wählen GL_RGBA, also 4 Komponenten.
			GL_UNSIGNED_BYTE,	//Unsere Texturdaten liegen als unsigned char vor. Ein char ist ein byte in C++, Also sagen wir GL_UNSIGNED_BYTE.
			image				//Schlussendlich der Pointer auf die rohen Texturdaten
		);

		if (checkglerror())		//Wenn das Format der Eingabe Textur nicht passte oder ein anderer Fehler gemacht wurde
		{
			glDeleteTextures(1, &texid);	//Geben wir das Texturobjekt wieder frei,
			SOIL_free_image_data(image);	//Geben den Speicher frei, den SOIL für die Texturdaten angelegt hat
			throw std::logic_error("Error. Could not buffer texture data.");	//Und werfen eine Exception
		}

		glGenerateMipmap(GL_TEXTURE_2D); GLERR	//Wir generieren unsere Mipmap

		glBindTexture(GL_TEXTURE_2D, 0); GLERR //An dieser Stelle sind wir fertig. Also unbinden wir das Texturbjekt indem wir 0 an GL_TEXTURE_2D binden.
		SOIL_free_image_data(image);  //Wichtig! Den Speicher, den SOIL für die Texturdaten reserviert hat wieder freigeben.
	}

	return std::make_shared<Texture2D>(texid);	//Wir konstruieren einen Shared Pointer auf ein Objekt unserer Texture2D Wrapper Klasse und geben diesen zurück.
}

//Erstellen eines VAO aus einem OBJMesh
std::shared_ptr<VAO> GLUtils::createVAO(const OBJMesh& mesh)
{
	//VAO Handle
	GLuint vao;
	//VAO erzeugen
	glGenVertexArrays(1, &vao); GLERR
	//Falls das VAO 0 ist, ist irgendetwas schief gegangen und wir werfen eine Exception
	if (vao == 0)
	{
		throw std::logic_error("VAO could not be created.");
	}

	//Vertex Buffer Handle
	GLuint vbo;
	//Vertex Buffer Object erzeugen
	glGenBuffers(1, &vbo);
	//Falls es danach 0 ist, ist irgendetwas schief gegangen.
	if (vbo == 0)
	{
		glDeleteVertexArrays(1, &vao);	//Wir geben das VAO von oben wieder frei
		throw std::logic_error("VBO could not be created.");	//Und werfen eine Exception.
	}

	//Index Buffer Handle
	GLuint ibo;
	//Index Buffer Object erzeugen
	glGenBuffers(1, &ibo);
	//Wenn ibo 0 ist, ist irgendetwas schief gegangen
	if (ibo == 0)
	{
		glDeleteVertexArrays(1, &vao);	//Wir geben das VAO von oben frei
		glDeleteBuffers(1, &vbo);		//Wir geben das VBO von oben frei
		throw std::logic_error("IBO could not be created.");	//Und werfen eine Exception
	}

	//VAO binden
	glBindVertexArray(vao);	GLERR
	//Vertex Buffer binden
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GLERR
	//Index Buffer binden
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); GLERR

	//Zu den Mesh Daten:
	//Ein Vertex sieht so aus:
	/*
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 tangent;
	};
	*/
	//Ein Index ist einfach nur ein GLuint Wert.
	//Das OBJMesh hat einen std::vector<Vertex> und einen std::vector<GLuint>.
	//Wir können einfach die kompletten Arrays in unsere Buffer schreiben, indem wir die data() Methode aufrufen und den Pointer auf die Daten an OpenGL übergeben.
	//OpenGL verlangt hier const void* Pointer. Wir casten den Pointer aus data() mithilfe von reinterpret_cast.
	//reinterpret_cast macht genaugenommen garnichts außer den Compiler anzulügen. Wir sagen dem Compiler hier,
	//Das der Pointer, der aus data() herauskommt ein Pointer auf const GLvoid ist.

	//Achtung! reinterpret_cast wird und sollte nur in Ausnahmefällen benutzt werden. Das hier ist eine solche Ausnahme!

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), reinterpret_cast<const GLvoid*>(mesh.vertices.data()), GL_STATIC_DRAW); GLERR

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * mesh.indices.size(), reinterpret_cast<const GLvoid*>(mesh.indices.data()), GL_STATIC_DRAW); GLERR

	//Jetzt müssen wir OpenGL beibringen, wie es unsere Daten zu interpretieren hat.

	//unsere Vertexdaten liegen ja wie folgt im Speicher:
	/*
	vec3 position	//3 floats
	vec2 uv			//2 floats
	vec3 normal		//3 floats
	vec3 tangent	//3 floats
	vec3 position
	vec2 uv
	vec3 normal
	vec3 tangent
	vec3 position
	vec2 uv
	vec3 normal
	vec3 tangent
	... usw
	*/
	//So eine "interleaved" Auslegung der Daten ist generell etwas schneller als separate Arrays, das hat etwas mit den Caches auf der GPU zu tun.
	//Grob gesagt müssen die GPU nicht so viel im Speicher herumsspringen weil die Daten für einen Vertex direkt hintereinander liegen.
	//Außerdem ist es ziemlich einfach die VertexAttribPointer dafür zu setzen.

	//position
	glVertexAttribPointer(
		0,					//Das erste Vertexattribut also 0
		3,					//position ist ein vec3, also 3 Komponenten
		GL_FLOAT,			//Die einzelnen Komponenten sind vom Typ float
		GL_FALSE,			//Die Daten sollen nicht normalisiert werden
		sizeof(Vertex),		//Der Anfang des jeweils nächsten Vertex ist liegt um sizeof(Vertex) hinter dem letzten. Dieser "stride" Parameter sagt aus, wie weit OpenGL im Speicher weitergehen muss um z.B. von einem position Vektor zum nächsten zu kommen.
		reinterpret_cast<void*>(offsetof(Vertex, position))	//Dieser Parameter gibt an, wie weit das Attribut position relativ zum Anfang der Datenstruktur Vertex verschoben ist.
															//Hier muss man glücklicherweise keine Bytes zählen, offsetof berechnet diesen Offset automatisch.
															//OpenGL verlangt hier einen void Pointer, daher casten wir den Offset wieder mit reinterpret_cast.
	); GLERR
	glEnableVertexAttribArray(0); GLERR	//Jetzt müssen wir noch das erste Vertexattribut "position" aktivieren. Hier
										//kommt also wieder die 0 rein.

	//Bei den restlichen Attributen tun wir genau das gleiche:
	//uv
	//Hier definieren wir jetzt das zweite Vertexattribut mit der Nummer 1 und aktivieren es.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv))); GLERR
	glEnableVertexAttribArray(1); GLERR

	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal))); GLERR
	glEnableVertexAttribArray(2); GLERR

	//tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent))); GLERR
	glEnableVertexAttribArray(3); GLERR

	//Wenn wir fertig sind, haben wir dem VAO gesagt, wo es die Daten findet und wie es die Daten zu interpretieren hat.
	//Beim Rendern muss der Vertex Buffer also nicht mehr gebunden sein, deswegen unbinden wir ihn hier:
	glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
	//Ein VAO speichert ja den Status der erzeugt wird, solange es gebunden ist. Dazu gehören Element Buffer Bindings und VertexAttribPointer.
	//VertexBuffer gehören nicht zum Status eines VAO. In OpenGL sollte man immer alles unbinden, was nicht mehr gebunden sein muss,
	//das hilft oft Fehler zu vermeiden.

	//Das VAO ist fertig und wir können es unbinden.
	glBindVertexArray(0); GLERR

	//Jetzt füllen wir nur noch unsere Wrapper Klasse und geben einen Shared Pointer darauf zurück.
	//Da vbo und ibo auch freigegeben werden müssen, kommen diese mit in die Klasse.
	//Die Anzahl der Indices müssen wir auch speichern, damit wir beim Zeichnen glDrawElements sagen können,
	//wie viele Vertices es zeichnen soll.
	return std::make_shared<VAO>(vbo, ibo, vao, mesh.indices.size());
}

//Einfachen Shader aus Vertex- und Fragment Shader laden und kompilieren
std::shared_ptr<ShaderProgram> GLUtils::createShaderProgram(const std::string& vspath, const std::string& fspath)
{
	//Handles für die beiden Shader und das fertige Programm
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	//Zuerst müssen wir den Code aus den Dateien einlesen.
	std::string vertexCode;			//string für den Vertex Shader Code
	std::string fragmentCode;		//string für den Fragment Shader Code
	std::ifstream vShaderFile;		//Input Filestream für das Vertex Shader File
	std::ifstream fShaderFile;		//Input Filestream für das Fragment Shader File
	//Wenn ein schwerwiegender Fehler beim Lesen auftritt, soll eine Exception geworfen werden:
	//(Mehr dazu in der C++ Doku.)
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try		//Beim Lesen von Dateien können immer Fehler auftreten. Deswegen verpacken wir das in einen Try Catch Block.
	{
		//Zunächst öfnen wir die Dateien. Wenn das fehlschlägt werden wir eine Exception.
		vShaderFile.open(vspath);
		if (!vShaderFile.is_open())
			throw std::invalid_argument("Vertex shader file not found.");
		fShaderFile.open(fspath);
		if (!fShaderFile.is_open())
			throw std::invalid_argument("Fragment shader file not found.");

		//Der Weg über einen stringstream ist eine angenehme Methode, eine ganze Datei aufeinmal auszulesen.
		//Vielleicht etwas langsam, aber wir tun das ja auch nur einmal.
		std::stringstream vShaderStream, fShaderStream;
		//Wir lesen den gesamten Inhalt der Dateien in die Stringstreams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//Und schließen die Filestreams.
		vShaderFile.close();
		fShaderFile.close();
		//Danach befüllen wir die Strings, die den Code enthalten sollen.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::exception& ex)
	{
		//Hier basteln wir uns eine einigermaßen sinnvolle Fehlermeldung zusammen und werfen eine etwas informativere Exception weiter nach oben.
		std::string errmsg;
		errmsg.append("Error: Shader files couldn't be read:\n");
		errmsg.append(ex.what());
		throw std::logic_error(errmsg.c_str());
	}

	//OpenGL verlangt C-Style char arrays. Wir machen das schonmal hier, dann vergisst man es im Wust des OpenGL bedingten C Codes später nicht.
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	//Diese beiden Variablen werden später gefüllt. Im infoLog steht zum Beispiel eine Fehlermeldung des GLSL Compilers
	//falls man Fehler in seinem Shader Code hat.
	GLint success;
	GLchar infoLog[512];

	//Kompilieren des Vertex Shaders
	//VertexShader Objekt erzeugen
	vertexShader = glCreateShader(GL_VERTEX_SHADER); GLERR
	//Wir sagen OpenGL wo der Sourcecode liegt, der zum Vertex Shader gehört
	glShaderSource(vertexShader, 1, &vShaderCode, NULL); GLERR
	//Und kompilieren den Code
	glCompileShader(vertexShader); GLERR
	//Falls Fehler aufgetreten sind, ist success nach diesem Aufruf false
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); GLERR
	if (!success)
	{
		//Wir holen die Fehlermeldung von OpenGL
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); GLERR
		//Bauen uns eine sinnvolle Fehlermeldung zusammen
		std::string errmsg;
		errmsg.append("Compiler error in vertex shader:\n");
		errmsg.append(infoLog);

		//Geben das Shader Objekt frei
		glDeleteShader(vertexShader); GLERR

		//Und werfen eine exception mit unserer Fehlermeldung
		throw std::logic_error(errmsg.c_str());
	}

	//Kompilieren des Fragment Shaders, genau wie beim Vertex Shader aber diesmal wird ein Fragment Shader kompiliert.
	//Hier erzeugen wir ein Fragment Shader Objekt. Alles andere ist wie beim Vertex Shader oben.
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); GLERR
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL); GLERR
	glCompileShader(fragmentShader); GLERR
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); GLERR
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); GLERR
		std::string errmsg;
		errmsg.append("Compiler error in fragment shader:\n");
		errmsg.append(infoLog);

		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR

		throw std::logic_error(errmsg.c_str());
	}

	// Wenn die beiden Shader erfolgreich kompiliert sind, können wir jetzt das Programm linken.
	//Program Objekt erzeugen
	program = glCreateProgram(); GLERR
	//Die beiden kompilierten Shader einhängen
	glAttachShader(program, vertexShader); GLERR
	glAttachShader(program, fragmentShader); GLERR
	//Und linken.
	glLinkProgram(program); GLERR
	//Wenn beim Linken ein Fehler aufgetreten ist, ist success nach diesem Aufruf wieder false.
	glGetProgramiv(program, GL_LINK_STATUS, &success); GLERR
	if (!success)
	{
		//Wir holen wieder die Fehlermeldung und bauen uns eine Sinnvolle Fehlermeldung für die Exception zusammen.
		glGetProgramInfoLog(program, 512, NULL, infoLog); GLERR
		std::string errmsg;
		errmsg.append("Linker error in program:\n");
		errmsg.append(infoLog);

		//Die Shader Objekte müssen wir jetzt freigeben, da sie sonst nutzlos den Speicher besetzen würden.
		//Dazu detachen wir als erstes die Shader Objekte von unserm Program Objekt
		glDetachShader(program, vertexShader); GLERR
		glDetachShader(program, fragmentShader); GLERR
		//und löschen anschließend die Shader Objekte
		glDeleteShader(vertexShader); GLERR
		glDeleteShader(fragmentShader); GLERR

		//Zum Schluss werfen wir die Exception mit unserer Fehlermeldung.
		throw std::logic_error(errmsg.c_str());
	}
	//Wenn alles erfolgreich kompiliert und gelinkt wurde brauchen wir die Shader Objekte nicht mehr.
	//Wir können sie also vom Program detachen und löschen.
	glDetachShader(program, vertexShader); GLERR
	glDetachShader(program, fragmentShader); GLERR
	glDeleteShader(vertexShader); GLERR
	glDeleteShader(fragmentShader); GLERR

	//Zum Schluss konstruieren wir das ShaderProgram Wrapper Objekt und geben einen Shared Pointer darauf zurück.
	return std::make_shared<ShaderProgram>(program);
}


//VAO wrapper
//Der Konstruktor nimmt VertexBuffer, IndexBuffer, Vertex Array Object und die Anzahl der Indizes und initialisiert
//damit die Member Variablen. VBO und IBO müssen ja ebenfalls gelöscht werden, daher kommen diese mit in
//unseren Wrapper.
VAO::VAO(GLuint vbo, GLuint ibo, GLuint vao, GLsizei indexct) :
	vbo(vbo),
	ibo(ibo),
	vao(vao),
	indexCount(indexct)
{

}

VAO::~VAO()
{	
	//Im Destruktor geben wir Index Buffer, Vertex Buffer und Vertex Array Object frei.
	glDeleteBuffers(1, &ibo);	GLERR
	glDeleteBuffers(1, &vbo);	GLERR
	glDeleteVertexArrays(1, &vao);	GLERR
}

void VAO::bind()
{
	//Falls vao ein gültiges OpenGL Objekt ist, binden wir es.
	if (vao != 0)
		glBindVertexArray(vao);	GLERR
}

void VAO::unbind()
{
	//Wir binden 0 um das VAO zu unbinden.
	glBindVertexArray(0); GLERR
}

	
//Texture2D Wrapper
//Der Konstruktor nimmt das OpenGl Texturobjekt entgegen und initialisiert damit die Membervariable
//Die Textureinheit intialisieren wir einfach mit 0.
Texture2D::Texture2D(GLuint texture) :
	tex(texture),
	tu(0)
{

}

Texture2D::~Texture2D()
{
	//Im Destruktor unbinden wir zunächst die Textur und löschen dann das Texturobjekt.
	unbind();
	glDeleteTextures(1, &tex); GLERR
}

void Texture2D::bind(GLuint textureUnit)
{
	//Wenn tex ein gültiges OpenGL Objekt ist
	if (tex != 0)
	{
		//Aktivieren wir die angefragte Textureinheit,
		glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit)); GLERR
		//Speichern die Textureinheit, auf das diese Textur gebunden wird,
		tu = textureUnit;
		//Und binden unsere Textur an die eben aktivierte Textureinheit.
		glBindTexture(GL_TEXTURE_2D, tex); GLERR
	}
}

void Texture2D::unbind()
{
	//glBindTexture bezieht sich immer auf die gerade aktive Textureinheit.
	//Man muss also sicherstellen, dass die richtige Textureinheit aktiv ist, wenn man die Textur
	//dort unbinden will.
	//Daher aktivieren wir zunächst die Textureinheit, auf der die Textur das letzte mal gebunden
	//wurde.
	glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(tu)); GLERR
	//Um die Textur zu unbinden, binden wir einfach 0.
	glBindTexture(GL_TEXTURE_2D, 0); GLERR	
}


//Shader Wrapper
//Der Konstruktor nimmt das Program Objekt entgegen und initialisiert damit die Membervariable
ShaderProgram::ShaderProgram(GLuint program) :
	prog(program)
{

}

ShaderProgram::~ShaderProgram()
{
	//Im Destruktor löschen wir das Program Objekt.
	glDeleteProgram(prog); GLERR
}

void ShaderProgram::use()
{
	//glUseProgram ist eine teure Operation. Zunächst fragen wir daher ab welches Program Objekt gerade gebunden ist.
	GLint current;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current); GLERR

	//Wenn unser Programm nicht schon gebunden ist und das Programm ein gültiges OpenGL Objekt ist, binden wir es.
	if (current != prog && prog != 0)
		glUseProgram(prog); GLERR
}
