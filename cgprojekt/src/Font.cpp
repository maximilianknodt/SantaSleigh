#include "Font.h"
#include <iostream>
#include <GL/glew.h>
#include FT_FREETYPE_H

Font::Font() {
	this->charactersMap = {};
	this->init();
}

int Font::init() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	// Abspeicherung der Schrift im 'face'
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	
	// Pro Pixel verwenden wir ein Byte -> unterschiedliche Groessen moeglich,
	// daher Sicherstellung, dass es keine Ausrichtungsproblem gibt
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Erstellung einer 8-Bit bmp Datei fuer die 128 ASCII Zeichen -> einfach halten
	for (unsigned char c = 0; c < 128; c++) {
		// Character Glype laden
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// Texture erstellen
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,	//TODO: andere/spezifische Farbwahl
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Textureoptionen einstellen
		// TODO: verstehen/nachschlagen
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
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Font::renderText(Shader& s, std::string text, float x, float y, float scale, Vector color) {
	s.Use();
	glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.X, color.Y, color.Z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// ueber alle Charactere itterieren
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = this->charactersMap[*c];

		float xpos = x + ch.bearingX * scale;
		float ypos = y - (ch.height - ch.bearingY) * scale;

		float w = ch.width * scale;
		float h = ch.height * scale;

		// fuer jeden Character VBO updaten
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		//
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
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}