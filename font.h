#include <cstdio>
#include <GL/glew.h>

#include "defines.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "libs/stb_truetype.h"

struct FontVertex {
    glm::vec2 position;
    glm::vec2 texCoords;
};

struct Font {
    ~Font() {
        if(fontVertexBufferData) {
            delete[]fontVertexBufferData;
        }
    }

    void initFont(const char* filename) {
        uint8 ttfBuffer[1<<20];
        uint8 tmpBitmap[512*512];

        fread(ttfBuffer, 1, 1<<20, fopen(filename, "rb"));
        stbtt_BakeFontBitmap(ttfBuffer, 0, 32.0f, tmpBitmap, 512, 512, 32, 96, cdata);

        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tmpBitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenVertexArrays(1, &fontVao);
        glBindVertexArray(fontVao);
        glGenBuffers(1, &fontVertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferId);
        fontVertexBufferCapacity = 20;
        fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
        glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, texCoords));
        glBindVertexArray(0);
    }

    void drawString(float x, float y, const char* text, Shader* fontShader) {
        glBindVertexArray(fontVao);
        glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferId);

        uint32 len = strlen(text);
        if(fontVertexBufferCapacity < len) {
            fontVertexBufferCapacity = len;
            glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
            delete[]fontVertexBufferData;
            fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
        }

        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, fontTexture));
        GLCALL(glUniform1i(glGetUniformLocation(fontShader->getShaderId(), "u_texture"), 0));

        FontVertex* vData = fontVertexBufferData;
        uint32 numVertices = 0;
        while(*text) {
            if(*text >= 32 && *text < 128) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(cdata, 512, 512, *text-32, &x, &y, &q, 1);
                
                vData[0].position = glm::vec2(q.x0, q.y0); vData[0].texCoords = glm::vec2(q.s0, q.t0);
                vData[1].position = glm::vec2(q.x1, q.y0); vData[1].texCoords = glm::vec2(q.s1, q.t0);
                vData[2].position = glm::vec2(q.x1, q.y1); vData[2].texCoords = glm::vec2(q.s1, q.t1);
                vData[3].position = glm::vec2(q.x0, q.y1); vData[3].texCoords = glm::vec2(q.s0, q.t1);
                vData[4].position = glm::vec2(q.x0, q.y0); vData[4].texCoords = glm::vec2(q.s0, q.t0);
                vData[5].position = glm::vec2(q.x1, q.y1); vData[5].texCoords = glm::vec2(q.s1, q.t1);

                vData += 6;
                numVertices += 6;
            }
            ++text;
        }

        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex)*numVertices, fontVertexBufferData));
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, numVertices));
    }

private:
    stbtt_bakedchar cdata[96];
    GLuint fontTexture;
    GLuint fontVao;
    GLuint fontVertexBufferId;
    FontVertex* fontVertexBufferData = 0;
    uint32 fontVertexBufferCapacity;
};

