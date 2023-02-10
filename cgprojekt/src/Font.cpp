#include "Font.h"
#include <iostream>
#include <GL/glew.h>

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Font::Font(unsigned int width, unsigned int height) {

	this->shader = BaseShader();
	if (this->shader.load(ASSET_DIRECTORY "vsText.glsl", ASSET_DIRECTORY "fsText.glsl") == false) {
		std::cout << "ERROR: Text Shader konnten nicht geladen werden" << std::endl;
	}

	this->shader.modelTransform(Matrix().orthographic(width, height, 0.0, 1.0));
	this->shader.setParameter(this->shader.getParameterID("projection"), this->shader.modelTransform());


	// VAO und VBO fuer die Text Quads einstellen
	// Generieren und Binden
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// Speicher des aktuell gebundenen (this->VBO) Buffers reservieren
	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 6 * 4), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	// Spezifizierung des Draw-Verhaltens des VBOs 
	// -> Typ = Float, nicht normalisieren, Byte-Abstand zwischen Vertex-Attributen
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	// VAO und VBO reseten
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->charactersMap = {};
}

void Font::load(std::string font, unsigned int fontSize) {
	this->charactersMap.clear();

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	// Abspeicherung der Schrift im 'face'
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize);
	
	// Pro Pixel verwenden wir ein Byte -> unterschiedliche Groessen moeglich,
	// daher Sicherstellung, dass es keine Ausrichtungsproblem gibt
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Erstellung einer 8-Bit bmp Datei fuer die 128 ASCII Zeichen -> einfach halten
	for (GLubyte c = 0; c < 128; c++) {
		// Character Glype laden
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// Texture erstellen
		unsigned int texture;
		// eine Textur erstellen und in 'texture' abspeichern
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Textureoptionen einstellen
		// S / T = U / V
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Texture in characters abspeichern
		Character character = {
			texture,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		this->charactersMap.insert(std::pair<char, Character>(c, character));
	}
	// FreeType aufloesen
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Font::renderText(BaseCamera& cam, std::string text, float x, float y, float scale, Color color) {
	// TODO: Shader rendern
	this->shader.activate(cam);
	this->shader.setParameter(this->shader.getParameterID("textColor"), color);
	//glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.X, color.Y, color.Z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// ueber alle Charactere itterieren
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = this->charactersMap[*c];

		float xpos = x + ch.bearingX * scale;
		float ypos = y + (this->charactersMap['H'].bearingY - ch.bearingY) * scale;
		// ----^ Achtung, zwei unterschiedliche Varianten, je nach opengl Seite

		float w = ch.width * scale;
		float h = ch.height * scale;

		// fuer jeden Character VBO updaten
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 0.0f },

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// VBO Speicherinhalt aktualisieren
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Quad rendern
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Cursor fuer naechste Glyphe ausrichten (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale;	// bitshift by 6 to get value in pixels (2^6 = 64)
	}
	// reseten
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}