#ifndef Font_hpp
#define Font_hpp

#include "ft2build.h"
#include FT_FREETYPE_H

#include <map>
#include "vector.h"
#include "BaseShader.h"

class Text : BaseShader {
public:
	struct Character{
		unsigned int textureID;
		float width;
		float height;			// gesamt
		float bearingX;			// Abstand Links
		float bearingY;			// Hoehe von Grundlinie aus
		unsigned int advance;	// gesamte Breite + Padding
	};

	Text(BaseCamera& cam, unsigned int width, unsigned int height);
	void renderText(BaseCamera& cam, std::string text, float x, float y, float scale, Color color);
	void loadFont(std::string font, unsigned int fontSize);

protected:
	std::map<char, Character> charactersMap;
	unsigned int VAO;
	unsigned int VBO;
	GLint ProjectionLoc;
};

#endif