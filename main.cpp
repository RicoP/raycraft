#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <map>
#include <vector>
#include <cmath>

#include "raylib/include/raylib.h"
#include "raylib/include/rlgl.h"
#include "raylib/include/raymath.h"
#include "perlin.cpp"
#include "camera.cpp"
#include "model.cpp"

Vector3 P3(float x, float y, float z);
Vector2 P2(float x, float y);
Vector3 P3(int x, int y, int z);
Vector2 P2(int x, int y);

const float CUBE = 1.f;
const float CAM_HEIGHT = 4 * CUBE;
const float JUMP_HEIGHT = 2 * CUBE;

enum CubeType { Dirt, Stone, Grass };

struct Cube {
    Vector3 position;
    Vector3 scales = P3(CUBE, CUBE, CUBE);
    Color color = BLACK;
    CubeType type = CubeType::Dirt;
};

struct Jump {
    float jumped_at = -1;
    float min_tall = 0;

    bool in_jump() { return this->jumped_at != -1; }

    float f(float t) {
        if (!this->in_jump()) return 0;
        float b = 2 * JUMP_HEIGHT / t;
        float a = -b / 2*t; 
        float ft = (a * pow(t, 2.0f)) + (b * t);
        if (ft <= 0) {
            this->jumped_at = -1;
            this->min_tall = 0;
            ft = 0;
        }
        return ft;
    }

    void start_jump(Camera3D &cam, float tallest_y) {
        if (this->in_jump()) return;
        this->jumped_at = GetTime();
        this->min_tall = tallest_y;
    }

    void update_jump(Camera &cam, float tallest_y = 0) {
        if (!this->in_jump()) return;
        cam.position.y = this->min_tall + CAM_HEIGHT + this->f(GetTime() - this->jumped_at);
        // height check
        if ((cam.position.y - CAM_HEIGHT) < tallest_y) {
            cam.position.y = tallest_y + CAM_HEIGHT;
            this->jumped_at = -1;
            this->min_tall = 0;
            return;
        }
    }
};

// static map
const int MAP_SIZE = 250;
std::vector<Cube> blocks;
void populateMap() {
    // random map generate
    for (int xx = 0; xx < MAP_SIZE; xx++) {
        for (int yy = 0; yy < MAP_SIZE; yy++) {
            int maximum_height = 0;
            for (int t = 0; t < (int)(perlin::perlin2d(xx, yy, 0.1, 1)*10); t++) {
                if (t > maximum_height) maximum_height = t;
                blocks.push_back(Cube {
                    .position= P3(xx, t, yy),
                    .type= t < 1 ? CubeType::Stone : CubeType::Dirt
                });
            }
            blocks.push_back(Cube {
                .position= P3(xx, (int)(maximum_height + CUBE), yy),
                .type= CubeType::Grass
            });
        }
    }
}

float getTallestY(float _x, float _z, bool tallest = true) {
    float r = -1.0f;
    int px = _x, pz = _z;
    for (const auto blk : blocks) {
        int cx = blk.position.x;
        int cy = blk.position.y;
        float cz = blk.position.z;
        // check block at that position
        if ( px == cx && pz == cz && cy > r ) {
            r = blk.position.y;
            if (!tallest) break;
        }
    }
    return r == -1 ? 0 : r;
}

int main()
{
    int scrWidth = 800, scrHeight = 600;
    SetTraceLogLevel(TraceLogLevel::LOG_WARNING);
    InitWindow(scrWidth, scrHeight, "Game");

    Image img0 = LoadImage("./resource/dirt_block.png");
    Image img1 = LoadImage("./resource/stone_block.png");
    Image img3 = LoadImage("./resource/grass_block.png");

    Texture DirtCubeTexture = LoadTextureFromImage(img0);
    Texture StoneCubeTexture = LoadTextureFromImage(img1);

    Image i_front = ImageFromImage(img3, Rectangle { .x= 0,.y= 0, .width= 16, .height= 16 });
    Image i_back = ImageFromImage(img3, Rectangle { .x= 16,.y= 0, .width= 16, .height= 16 });
    Image i_top = ImageFromImage(img3, Rectangle { .x= 32,.y= 0, .width= 16, .height= 16 });
    Image i_bottom = ImageFromImage(img3, Rectangle { .x= 48,.y= 0, .width= 16, .height= 16 });
    Image i_right = ImageFromImage(img3, Rectangle { .x= 64,.y= 0, .width= 16, .height= 16 });
    Image i_left = ImageFromImage(img3, Rectangle { .x= 80,.y= 0, .width= 16, .height= 16 });

    Texture t_front = LoadTextureFromImage(i_front);
    Texture t_back = LoadTextureFromImage(i_back);
    Texture t_top = LoadTextureFromImage(i_top);
    Texture t_bottom = LoadTextureFromImage(i_bottom);
    Texture t_right = LoadTextureFromImage(i_right);
    Texture t_left = LoadTextureFromImage(i_left);

    unsigned int GrassCubeTexture[6] = {
        t_front.id, t_back.id,
        t_top.id, t_bottom.id,
        t_right.id, t_left.id
    };

    UnloadImage(img0);
    UnloadImage(img1);
    UnloadImage(img3);

    populateMap();

    Jump jm;
    Camera3D C = {
        .position= P3(MAP_SIZE / 2.0f, CAM_HEIGHT, MAP_SIZE / 2.0f),
        .target= P3(0, 0, 0),
        .up= P3(0.f, CAM_HEIGHT, 0.f),
        .fovy= 60.0f,
        .projection= CameraProjection::CAMERA_PERSPECTIVE
    };

    EnableFirstPerson(C);
    SetTargetFPS(60);

    float speed, pointer_dm;
    bool free_observe = false;
    float tallest_y = 0;
    int draw_distance = 15;
    int drawn_blocks = 0;
    float respawn_msg = 0.0f;
    Ray mray;

    C.position.y = getTallestY(C.position.x, C.position.z) + CAM_HEIGHT;

    while (!WindowShouldClose())
    {
        //draw distance control
        if (IsKeyPressed(KEY_KP_ADD)) draw_distance++; //draw_distance += CAM_HEIGHT;
        if (IsKeyPressed(KEY_KP_SUBTRACT) && draw_distance > CAM_HEIGHT) draw_distance--; //draw_distance -= 1;

        // highest y in current (x,z)
        tallest_y = getTallestY(C.position.x, C.position.z);

        if ( !free_observe && ((int)tallest_y == 0) || IsKeyPressed(KEY_R)) {
            C.position.x = C.position.z = (int)MAP_SIZE / 2.0f;
            C.position.y = tallest_y = getTallestY(C.position.x, C.position.z);
            respawn_msg = GetTime();
        }

        // toggle free observe mode
        if (IsKeyPressed(KEY_F)) free_observe = !free_observe;

        pointer_dm = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 4 : 1.5;
        speed = IsKeyDown(KEY_LEFT_SHIFT) ? 3 : 5;

        if (IsKeyPressed(KEY_SPACE) && !jm.in_jump()) jm.start_jump(C, tallest_y);

        // if there is an active jump action
        if (jm.in_jump()) jm.update_jump(C, tallest_y);
        
        //  check for the least possible y
        //  keep him on the highest ground if character is not jumping
        if (!free_observe && !jm.in_jump() && (C.position.y < CAM_HEIGHT || C.position.y > CAM_HEIGHT)) {
            C.position.y = CAM_HEIGHT + tallest_y;
        }

        FirstPersonCamera(&C, false, speed);
        //lastpos = C.position;

        BeginDrawing();
        {
            ClearBackground(WHITE);

            BeginMode3D(C);
            {                
                mray = GetMouseRay(P2(scrWidth /2, scrHeight /2), C);
                RayCollision col = RayCollision { .hit= false };
                bool q = true;
                drawn_blocks = 0;
                int index = 0;
                for (const auto cb : blocks)
                {
                    int dist = Vector3Distance(C.position, cb.position);

                    if (dist > draw_distance) {
                        index++;
                        continue;
                    }
                    Vector3 adp = P3(cb.position.x + cb.scales.y / 2,cb.position.y + cb.scales.x / 2,cb.position.z + cb.scales.z / 2);

                    // dist > CAM_HEIGHT
                    if (q && dist < 6*CUBE) {
                        col = GetRayCollisionBox(mray, BoundingBox {
                            .min= P3(cb.position.x - cb.scales.x/2, cb.position.y - cb.scales.y/2, cb.position.z - cb.scales.z/2 ),
                            .max= P3(cb.position.x + cb.scales.x/2, cb.position.y + cb.scales.y/2, cb.position.z + cb.scales.z/2 )
                        });
                        if (
                            col.hit &&
                            (int)floor(mray.direction.x) == 0 &&
                            ((int)(mray.direction.y)) <= -1.1 &&
                            (int)floor(mray.direction.z) == 0
                        ) {
                            DrawCubeWires(P3(C.position.x, tallest_y, C.position.z), CUBE, CUBE, CUBE, LIGHTGRAY);
                        }
                        q = !col.hit;
                    }
					Color c = WHITE;
                    if (col.hit) {
                        DrawCubeWires(adp, CUBE, CUBE,CUBE, LIGHTGRAY);
						c = LIGHTGRAY;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && cb.type != CubeType::Stone) {
                            blocks.erase(blocks.begin() + index);
                        }
                        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                            blocks.push_back(Cube {
                                .position= P3(cb.position.x, cb.position.y + CUBE, cb.position.z)
                            });
                        }
                    }
                    switch (cb.type) {                    
                        case CubeType::Dirt: DrawCubeTexture(DirtCubeTexture, adp, CUBE, CUBE, CUBE, c); break;
                        case CubeType::Stone: DrawCubeTexture(StoneCubeTexture, adp, CUBE, CUBE, CUBE, c); break;
                        case CubeType::Grass: CUSTOM_DrawCubeTexture(GrassCubeTexture, adp, CUBE, CUBE, CUBE, c); break;
                        default: DrawCube(adp, cb.scales.x, cb.scales.y, cb.scales.z, LIGHTGRAY); break;
                    }

                    if (!q) col.hit = false;

                    drawn_blocks++;
                    index++;
                }
            }
            EndMode3D();

            DrawText(TextFormat("P{.x= %.2f, .y= %.2f, z: %.2f}\nT{.x= %.2f, .y= %.2f, z: %.2f}\nSpeed: %.1f\nBlocks: %i of %i\nDistance: %i", C.position.x, C.position.y, C.position.z, C.target.x, C.target.y, C.target.z, speed, drawn_blocks, blocks.size(), draw_distance), 20, 20, 8, GRAY);
            DrawText(TextFormat("%i fps", GetFPS()), (scrWidth / 2) - 20, 10, 10, BLACK);

            DrawCircle(scrWidth / 2, scrHeight / 2, pointer_dm, ColorAlpha(BLACK, 0.3));
            if (free_observe) DrawText("FREE OBSERVER MODE", 20, scrHeight - 20, 10, RED);
            if (jm.in_jump()) DrawText("JUMP", scrWidth - 40, 10, 10, RED);
            if ( (GetTime() - respawn_msg) < 3) {
                DrawRectangle(0,0,scrWidth, scrHeight, ColorAlpha(BLACK, 0.3));
                DrawText("RESPAWN", (scrWidth / 2) - 50, scrHeight / 2, 25, RED);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

Vector3 P3(float x, float y, float z) { return Vector3{.x= x, .y= y, .z= z}; }
Vector2 P2(float x, float y) { return Vector2{.x= x, .y= y}; };
Vector3 P3(int x, int y, int z) { return P3((float)x, (float)y, (float)z); }
Vector2 P2(int x, int y) { return P2((float)x, (float)y); }
