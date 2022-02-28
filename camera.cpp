// Custom first-person camera handling
#include <cmath>
#include "raylib/include/raylib.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef PI
	#define PI 3.14159265358979323846
#endif

#ifndef DEG2RAD
	#define DEG2RAD (PI / 180.0f)
#endif

// Camera mouse movement sensitivity
#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f

// FREE_CAMERA
#define CAMERA_FREE_MOUSE_SENSITIVITY 0.01f
#define CAMERA_FREE_DISTANCE_MIN_CLAMP 0.3f
#define CAMERA_FREE_DISTANCE_MAX_CLAMP 120.0f
#define CAMERA_FREE_MIN_CLAMP 85.0f
#define CAMERA_FREE_MAX_CLAMP -85.0f
#define CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY 0.05f
#define CAMERA_FREE_PANNING_DIVIDER 5.1f

// ORBITAL_CAMERA
#define CAMERA_ORBITAL_SPEED 0.01f // Radians per frame

// FIRST_PERSON
//#define CAMERA_FIRST_PERSON_MOUSE_SENSITIVITY           0.003f
#define CAMERA_FIRST_PERSON_FOCUS_DISTANCE 25.0f
#define CAMERA_FIRST_PERSON_MIN_CLAMP 89.0f
#define CAMERA_FIRST_PERSON_MAX_CLAMP -89.0f

#define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER 8.0f
#define CAMERA_FIRST_PERSON_STEP_DIVIDER 30.0f
#define CAMERA_FIRST_PERSON_WAVING_DIVIDER 200.0f

// THIRD_PERSON
//#define CAMERA_THIRD_PERSON_MOUSE_SENSITIVITY           0.003f
#define CAMERA_THIRD_PERSON_DISTANCE_CLAMP 1.2f
#define CAMERA_THIRD_PERSON_MIN_CLAMP 5.0f
#define CAMERA_THIRD_PERSON_MAX_CLAMP -85.f
#define CAMERA_THIRD_PERSON_OFFSET \
	(Vector3) { 0.4f, 0.0f, 0.0f }

// PLAYER (used by camera)
//#define PLAYER_MOVEMENT_SENSITIVITY 14.0f

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Camera move modes (first person and third person cameras)
typedef enum
{
	MOVE_FRONT = 0,
	MOVE_BACK,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN
} CameraMove;

// Camera global state context data [56 bytes]
typedef struct
{
	unsigned int mode;			   // Current camera mode
	float targetDistance;		   // Camera distance from position to target
	float playerEyesPosition;	   // Player eyes position from ground (in meters)
	Vector2 angle;				   // Camera angle in plane XZ
	Vector2 previousMousePosition; // Previous mouse position

	// Camera movement control keys
	int moveControl[6];	   // Move controls (CAMERA_FIRST_PERSON)
	int smoothZoomControl; // Smooth zoom control key
	int altControl;		   // Alternative control key
	int panControl;		   // Pan view control key
} CameraData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static CameraData CAMERA = {
	// Global CAMERA state context
	.mode = 0,
	.targetDistance = 0,
	.playerEyesPosition = 1.85f,
	.angle = {0},
	.previousMousePosition = {0},
	.moveControl = {'W', 'S', 'D', 'A', 'E', 'Q'},
	.smoothZoomControl = 341, // raylib: KEY_LEFT_CONTROL
	.altControl = 342,		  // raylib: KEY_LEFT_ALT
	.panControl = 2			  // raylib: MOUSE_BUTTON_MIDDLE
};

// CUSTOM handle
void EnableFirstPerson(Camera camera/*, int mode*/)
{
    Vector3 v1 = camera.position;
    Vector3 v2 = camera.target;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;

    CAMERA.targetDistance = sqrtf(dx*dx + dy*dy + dz*dz);   // Distance to target

    // Camera angle calculation
    CAMERA.angle.x = atan2f(dx, dz);                        // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
    CAMERA.angle.y = atan2f(dy, sqrtf(dx*dx + dz*dz));      // Camera angle in plane XY (0 aligned with X, move positive CW)

    CAMERA.playerEyesPosition = camera.position.y;          // Init player eyes position to camera Y position

    CAMERA.previousMousePosition = GetMousePosition();      // Init mouse position

    // Lock cursor for first person and third person cameras
    /*if ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON))*/ DisableCursor();
    //else EnableCursor();

    CAMERA.mode = CAMERA_FIRST_PERSON;
}

// CUSTOM handle
void FirstPersonCamera(Camera *camera, bool force_eyedist = false, float mv_sensivity = 25.f)
{
	int swingCounter = 0; // Used for 1st person swinging movement

	// TODO: Compute CAMERA.targetDistance and CAMERA.angle here (?)

	// Mouse movement detection
	Vector2 mousePositionDelta = {0.0f, 0.0f};
	Vector2 mousePosition = GetMousePosition();
	float mouseWheelMove = GetMouseWheelMove();

	// Keys input detection
	// TODO: Input detection is raylib-dependant, it could be moved outside the module
	bool keyPan = IsMouseButtonDown(CAMERA.panControl);
	bool keyAlt = IsKeyDown(CAMERA.altControl);
	bool szoomKey = IsKeyDown(CAMERA.smoothZoomControl);
	bool direction[6] = {IsKeyDown(CAMERA.moveControl[MOVE_FRONT]),
						 IsKeyDown(CAMERA.moveControl[MOVE_BACK]),
						 IsKeyDown(CAMERA.moveControl[MOVE_RIGHT]),
						 IsKeyDown(CAMERA.moveControl[MOVE_LEFT]),
						 // these two are reserved for jump action
						 /*IsKeyDown(CAMERA.moveControl[MOVE_UP])*/ false,
						 /*IsKeyDown(CAMERA.moveControl[MOVE_DOWN])*/ false};

	// commented this and the app stopped working :)
	if (CAMERA.mode != CAMERA_CUSTOM)
	{
		mousePositionDelta.x = mousePosition.x - CAMERA.previousMousePosition.x;
		mousePositionDelta.y = mousePosition.y - CAMERA.previousMousePosition.y;
	
		CAMERA.previousMousePosition = mousePosition;
	}

	camera->position.x += (sinf(CAMERA.angle.x) * direction[MOVE_BACK] -
						   sinf(CAMERA.angle.x) * direction[MOVE_FRONT] -
						   cosf(CAMERA.angle.x) * direction[MOVE_LEFT] +
						   cosf(CAMERA.angle.x) * direction[MOVE_RIGHT]) /
						  mv_sensivity;

	camera->position.y += (sinf(CAMERA.angle.y) * direction[MOVE_FRONT] -
						   sinf(CAMERA.angle.y) * direction[MOVE_BACK] +
						   1.0f * direction[MOVE_UP] - 1.0f * direction[MOVE_DOWN]) /
						  mv_sensivity;

	camera->position.z += (cosf(CAMERA.angle.x) * direction[MOVE_BACK] -
						   cosf(CAMERA.angle.x) * direction[MOVE_FRONT] +
						   sinf(CAMERA.angle.x) * direction[MOVE_LEFT] -
						   sinf(CAMERA.angle.x) * direction[MOVE_RIGHT]) /
						  mv_sensivity;

	// Camera orientation calculation
	CAMERA.angle.x += (mousePositionDelta.x * -CAMERA_MOUSE_MOVE_SENSITIVITY);
	CAMERA.angle.y += (mousePositionDelta.y * -CAMERA_MOUSE_MOVE_SENSITIVITY);

	// Angle clamp
	if (CAMERA.angle.y > CAMERA_FIRST_PERSON_MIN_CLAMP * DEG2RAD)
		CAMERA.angle.y = CAMERA_FIRST_PERSON_MIN_CLAMP * DEG2RAD;
	else if (CAMERA.angle.y < CAMERA_FIRST_PERSON_MAX_CLAMP * DEG2RAD)
		CAMERA.angle.y = CAMERA_FIRST_PERSON_MAX_CLAMP * DEG2RAD;

	// Calculate translation matrix
	Matrix matTranslation = {1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, (CAMERA.targetDistance / CAMERA_FREE_PANNING_DIVIDER),
							 0.0f, 0.0f, 0.0f, 1.0f};

	// Calculate rotation matrix
	Matrix matRotation = {1.0f, 0.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f,
						  0.0f, 0.0f, 0.0f, 1.0f};

	float cosz = cosf(0.0f);
	float sinz = sinf(0.0f);
	float cosy = cosf(-(PI * 2 - CAMERA.angle.x));
	float siny = sinf(-(PI * 2 - CAMERA.angle.x));
	float cosx = cosf(-(PI * 2 - CAMERA.angle.y));
	float sinx = sinf(-(PI * 2 - CAMERA.angle.y));

	matRotation.m0 = cosz * cosy;
	matRotation.m4 = (cosz * siny * sinx) - (sinz * cosx);
	matRotation.m8 = (cosz * siny * cosx) + (sinz * sinx);
	matRotation.m1 = sinz * cosy;
	matRotation.m5 = (sinz * siny * sinx) + (cosz * cosx);
	matRotation.m9 = (sinz * siny * cosx) - (cosz * sinx);
	matRotation.m2 = -siny;
	matRotation.m6 = cosy * sinx;
	matRotation.m10 = cosy * cosx;

	// Multiply translation and rotation matrices
	Matrix matTransform = {0};
	matTransform.m0 = matTranslation.m0 * matRotation.m0 + matTranslation.m1 * matRotation.m4 + matTranslation.m2 * matRotation.m8 + matTranslation.m3 * matRotation.m12;
	matTransform.m1 = matTranslation.m0 * matRotation.m1 + matTranslation.m1 * matRotation.m5 + matTranslation.m2 * matRotation.m9 + matTranslation.m3 * matRotation.m13;
	matTransform.m2 = matTranslation.m0 * matRotation.m2 + matTranslation.m1 * matRotation.m6 + matTranslation.m2 * matRotation.m10 + matTranslation.m3 * matRotation.m14;
	matTransform.m3 = matTranslation.m0 * matRotation.m3 + matTranslation.m1 * matRotation.m7 + matTranslation.m2 * matRotation.m11 + matTranslation.m3 * matRotation.m15;
	matTransform.m4 = matTranslation.m4 * matRotation.m0 + matTranslation.m5 * matRotation.m4 + matTranslation.m6 * matRotation.m8 + matTranslation.m7 * matRotation.m12;
	matTransform.m5 = matTranslation.m4 * matRotation.m1 + matTranslation.m5 * matRotation.m5 + matTranslation.m6 * matRotation.m9 + matTranslation.m7 * matRotation.m13;
	matTransform.m6 = matTranslation.m4 * matRotation.m2 + matTranslation.m5 * matRotation.m6 + matTranslation.m6 * matRotation.m10 + matTranslation.m7 * matRotation.m14;
	matTransform.m7 = matTranslation.m4 * matRotation.m3 + matTranslation.m5 * matRotation.m7 + matTranslation.m6 * matRotation.m11 + matTranslation.m7 * matRotation.m15;
	matTransform.m8 = matTranslation.m8 * matRotation.m0 + matTranslation.m9 * matRotation.m4 + matTranslation.m10 * matRotation.m8 + matTranslation.m11 * matRotation.m12;
	matTransform.m9 = matTranslation.m8 * matRotation.m1 + matTranslation.m9 * matRotation.m5 + matTranslation.m10 * matRotation.m9 + matTranslation.m11 * matRotation.m13;
	matTransform.m10 = matTranslation.m8 * matRotation.m2 + matTranslation.m9 * matRotation.m6 + matTranslation.m10 * matRotation.m10 + matTranslation.m11 * matRotation.m14;
	matTransform.m11 = matTranslation.m8 * matRotation.m3 + matTranslation.m9 * matRotation.m7 + matTranslation.m10 * matRotation.m11 + matTranslation.m11 * matRotation.m15;
	matTransform.m12 = matTranslation.m12 * matRotation.m0 + matTranslation.m13 * matRotation.m4 + matTranslation.m14 * matRotation.m8 + matTranslation.m15 * matRotation.m12;
	matTransform.m13 = matTranslation.m12 * matRotation.m1 + matTranslation.m13 * matRotation.m5 + matTranslation.m14 * matRotation.m9 + matTranslation.m15 * matRotation.m13;
	matTransform.m14 = matTranslation.m12 * matRotation.m2 + matTranslation.m13 * matRotation.m6 + matTranslation.m14 * matRotation.m10 + matTranslation.m15 * matRotation.m14;
	matTransform.m15 = matTranslation.m12 * matRotation.m3 + matTranslation.m13 * matRotation.m7 + matTranslation.m14 * matRotation.m11 + matTranslation.m15 * matRotation.m15;

	camera->target.x = camera->position.x - matTransform.m12;
	camera->target.y = camera->position.y - matTransform.m13;
	camera->target.z = camera->position.z - matTransform.m14;

	// If movement detected (some key pressed), increase swinging
	/*for (int i = 0; i < 6; i++)
		if (direction[i])
		{
			//swingCounter++;
			break;
		}*/

	// Camera position update
	// NOTE: On CAMERA_FIRST_PERSON player Y-movement is limited to player 'eyes position'
	if(force_eyedist) camera->position.y = CAMERA.playerEyesPosition - sinf(swingCounter / CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER) / CAMERA_FIRST_PERSON_STEP_DIVIDER;

	camera->up.x = sinf(swingCounter / (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / CAMERA_FIRST_PERSON_WAVING_DIVIDER;
	camera->up.z = -sinf(swingCounter / (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / CAMERA_FIRST_PERSON_WAVING_DIVIDER;
}
