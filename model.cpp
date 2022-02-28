#include <cmath>
#include "raylib/include/raylib.h"
#include "raylib/include/raymath.h"
#include "raylib/include/rlgl.h"

void CUSTOM_DrawCubeTexture(unsigned int T[6], Vector3 position, float width, float height, float length, Color color) {
    float x = position.x, y = position.y, z = position.z;
    rlCheckRenderBatchLimit(36);

    rlSetTexture(T[0]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Front Face
		rlNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing Towards Viewer
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
	rlEnd();
    
	rlSetTexture(T[1]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Back Face
		rlNormal3f(0.0f, 0.0f, - 1.0f);                  // Normal Pointing Away From Viewer
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
	rlEnd();

	rlSetTexture(T[2]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Top Face
		rlNormal3f(0.0f, 1.0f, 0.0f);                  // Normal Pointing Up
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left Of The Texture and Quad
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right Of The Texture and Quad
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
	rlEnd();

	rlSetTexture(T[3]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Bottom Face
		rlNormal3f(0.0f, - 1.0f, 0.0f);                  // Normal Pointing Down
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Right Of The Texture and Quad
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Left Of The Texture and Quad
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
	rlEnd();

	rlSetTexture(T[4]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Right face
		rlNormal3f(1.0f, 0.0f, 0.0f);                  // Normal Pointing Right
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
	rlEnd();

	rlSetTexture(T[5]);
	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		// Left Face
		rlNormal3f( - 1.0f, 0.0f, 0.0f);                  // Normal Pointing Left
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
	rlEnd();

	rlSetTexture(0);
}
