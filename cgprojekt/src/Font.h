#ifndef Font_hpp
#define Font_hpp

#include "ft2build.h"
#include <map>
#include "vector.h"

class Font {
public:
	struct Character{
		unsigned int textureID;
		float width;
		float height;		// gesamt
		float bearingX;		// Abstand Links
		float bearingY;		// Hoehe von Grundlinie aus
		//glm::ivec2 size; glm::ivec2 bearing;
		unsigned int advance;	// gesamte Breite + Padding
	};
	Font();

protected:
	int init();
	// void loadCharacters(FT_Library &ft, FT_Face &face);
	void renderText(Shader& s, std::string text, float x, float y, float scale, Vector color);

	std::map<char, Character> charactersMap;
	unsigned int VAO;
	unsigned int VBO;
};

#endif