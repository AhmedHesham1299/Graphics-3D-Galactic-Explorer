#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <Windows.h>
#include <PlaySoundAPI.h>
#include <sstream>
#define M_PI 3.14159265358979323846

char title[] = "Galactic Explorer";
int WIDTH = 1280;
int HEIGHT = 720;
bool  won = false;
float astronautPosX = 0.0f;
float astronautPosY = 3.0f;
float astronautPosZ = 0.0f;
float angle = 0.0f;
float asteroidPosX[3] = { -10.0,10.0,0.0 };
float asteroidPosY[3] = { 3.0,3.0,3.0 };
float asteroidPosZ[3] = { 0.0,0.0,10.0 };
float asteroidRadius[3] = { 2.0, 2.0, 2.0 };
char scoreText[50];
int score = 0;
bool crystalCollected[3] = { false, false, false };
float crystalPosX[3] = { -3.0,-10.0,10.0 };
float crystalPosY[3] = { 0.0,0.0,0.0 };
float crystalPosZ[3] = { -9.0,10.0,10.0, };
float crystalRadius[4] = { 2.0, 2.0, 2.0, 2.0 };
bool level1 = true;
bool isMoonTexture = true;
float astronautVelY = 0.0f;
float cloudPosX[3] = { -10.0,10.0,0.0 };
float cloudPosY[3] = { 3.0,3.0,3.0 };
float cloudPosZ[3] = { 0.0,0.0,10.0 };
float cloudRadius[3] = { 2.0, 2.0, 2.0 };
bool tresureCollected[3] = { false, false, false };
float tresurePosX[3] = { -4.20,14.0,8.0 };
float tresurePosY[3] = { 0.1,0.1,0.1 };
float tresurePosZ[3] = { -3.40,-11.0,14.0 };
float tresureRadius[3] = { 2.0, 2.0, 2.0 };
float astronautRotation = 0.0f;
float cameraPosX;
float cameraPosY;
float cameraPosZ;
float cameraTargetX;
float cameraTargetY;
float cameraTargetZ;
float rotX = 0.0f;
float rotY = 0.0f;
const float PI = 3.14159f;
bool isFirstPersonView = false;
GLuint LoadTexture(const char* filename) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	return textureID;
}
GLuint gameOverTexture = LoadTexture("gameover.bmp");



GLuint tex, tex2;

GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;




class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;

	}
};

Vector Eye(20, 5, 20);
Vector At(-10, 0, -10);
Vector Up(0, 1, 0);
Vector cometPos(0.0f, 10.0f, 0.0f);

int cameraZoom = 0;

Model_3DS model_Crystal;
Model_3DS model_Asteriod;
Model_3DS model_WarmHole;
Model_3DS model_astronaut;
Model_3DS model_Spacetreasure;
Model_3DS model_NebulaCloud;
Model_3DS model_Portal;

GLTexture tex_ground;
GLTexture tex_jupiter;
GLuint tex_ground1;
GLuint tex_jupiter1;
//=======================================================================
// Animation Function
//=======================================================================


//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void updateLight() {
	time_t t = time(NULL);
	struct tm* now = localtime(&t);

	float intensity = cos((now->tm_sec / 10.0f) * 2 * M_PI) * 0.5f + 0.5f;

	GLfloat lightColor[4];
	if (level1) {
		lightColor[0] = intensity;
		lightColor[1] = intensity * 0.5f;
		lightColor[2] = 0.0f;
	}
	else {
		lightColor[0] = 0.0f;
		lightColor[1] = intensity * 0.5f;
		lightColor[2] = intensity;
	}
	lightColor[3] = 1.0f;

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	cometPos.x += 0.1f;

	if (cometPos.x > 20.0f) {
		cometPos.x = -20.0f;
	}

	GLfloat lightPos[] = { cometPos.x, cometPos.y, cometPos.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderMoon()
{
	glDisable(GL_LIGHTING);


	glEnable(GL_TEXTURE_2D);

	if (level1) {
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);

	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex_jupiter.texture[0]);

	}

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-17, 0, -17);
	glTexCoord2f(5, 0);
	glVertex3f(17, 0, -17);
	glTexCoord2f(5, 5);
	glVertex3f(17, 0, 17);
	glTexCoord2f(0, 5);
	glVertex3f(-17, 0, 17);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	glColor3f(1, 1, 1);
	float minX = -15.0f;
	float maxX = 15.0f;
	float minZ = -15.0f;
	float maxZ = 15.0f;

	if (astronautPosX < minX)
		astronautPosX = minX;
	else if (astronautPosX > maxX)
		astronautPosX = maxX;

	if (astronautPosZ < minZ)
		astronautPosZ = minZ;
	else if (astronautPosZ > maxZ)
		astronautPosZ = maxZ;
}


//=======================================================================
// Display Function
//=======================================================================
void Anim()
{
	updateLight();

	angle += 1.0f;
	if (angle >= 360.0f) {
		angle -= 360.0f;
	}
	astronautPosY += astronautVelY * 0.1f;
	astronautVelY -= 0.1f;

	if (astronautPosY < 3.0f) {
		astronautPosY = 3.0f;
		astronautVelY = 0.0f;
	}
	glutPostRedisplay();
}
void print(int x, int y, int z, char* string) {
	int len, i;

	glRasterPos3f(x, y, z);
	len = (int)strlen(string);

	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (level1 && isMoonTexture) {
		RenderMoon();

		for (int i = 0; i < 3; ++i) {
			if (!crystalCollected[i]) {
				glPushMatrix();
				glTranslatef(crystalPosX[i], crystalPosY[i], crystalPosZ[i]);
				glRotatef(90.0f, 1, 0, 0);
				glRotatef(angle, 0, 0, 1);
				glScalef(0.9, 0.9, 0.9);

				model_Crystal.Draw();
				glPopMatrix();
			}
		}


		glPushMatrix();
		glTranslatef(-10.0, 3.0, 0.0);
		glRotatef(90.0f, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.01, 0.01, 0.01);

		model_Asteriod.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(10.0, 3.0, 0.0);
		glRotatef(90.0f, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.0099, 0.0099, 0.0099);

		model_Asteriod.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 3.0, 10.0);
		glRotatef(90.0f, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.01, 0.01, 0.01);

		model_Asteriod.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(astronautPosX, astronautPosY, astronautPosZ);
		glRotatef(90.f, 0, 1, 0);
		glRotatef(astronautRotation, 0.0f, 1.0f, 0.0f);
		glScalef(0.02, 0.02, 0.02);
		model_astronaut.Draw();


		glPopMatrix();

		glPushMatrix();
		glRotatef(90.f, 1, 0, 0);
		glTranslatef(0.0, -20.0, -6);
		glRotatef(angle, 0, 1, 0);

		model_WarmHole.Draw();
		glPopMatrix();


		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);

		glPopMatrix();

		glPushMatrix();
		glDisable(GL_LIGHTING);

		glPopMatrix();

	}
	else if (!level1 && !isMoonTexture) {
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);

		RenderMoon();


		for (int i = 0; i < 3; ++i) {
			if (!tresureCollected[i]) {
				glPushMatrix();
				glTranslatef(tresurePosX[i], tresurePosY[i], tresurePosZ[i]);
				glRotatef(angle, 0, 1, 0);
				glScalef(2.5, 2.5, 2.5);

				model_Spacetreasure.Draw();
				glPopMatrix();
			}
		}

		glPushMatrix();
		glTranslatef(-10.0, 3.0, 0.0);
		glRotatef(90.0f, 1, 0, 0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.003, 0.003, 0.003);

		model_NebulaCloud.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(10.0, 3.0, 0.0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.004, 0.004, 0.004);

		model_NebulaCloud.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 3.0, 10.0);
		glRotatef(angle, 0, 0, 1);
		glScalef(0.004, 0.004, 0.004);

		model_NebulaCloud.Draw();
		glPopMatrix();



		glPushMatrix();
		glTranslatef(astronautPosX, astronautPosY, astronautPosZ);
		glRotatef(90.f, 0, 1, 0);
		glRotatef(astronautRotation, 0.0f, 1.0f, 0.0f);
		glScalef(0.02, 0.02, 0.02);

		model_astronaut.Draw();
		glPopMatrix();

		glPushMatrix();
		glScalef(1.3, 1.3, 1.3);

		glRotatef(90.f, 1, 0, 0);
		glTranslatef(0.0, -25.0, -6);
		glRotatef(angle, 0, 1, 0);

		glScalef(10, 10, 10);

		model_Portal.Draw();
		glPopMatrix();


		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos3f(-30.0, 8.0, 10.0);

	sprintf(scoreText, "Score: %d", score);

	for (char c : scoreText)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glPopAttrib();

	char buffer[100];
	sprintf(buffer, "Player Coordinates: (%.2f, %.2f, %.2f)\n", astronautPosX, astronautPosY, astronautPosZ);
	OutputDebugStringA(buffer);

	/*if (won) {
		// Enable 2D texturing
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gameOverTexture);

		// Draw a textured quad
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
		glEnd();

		// Disable 2D texturing
		glDisable(GL_TEXTURE_2D);
	}*/


	glutSwapBuffers();
}


//=======================================================================
// Keyboard Function
//=======================================================================
bool isColliding(float x1, float y1, float z1, float r1, float x2, float y2, float z2, float r2) {
	float distSq = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2);
	float radiusSum = r1 + r2;
	bool isColliding = distSq <= radiusSum * radiusSum;
	return isColliding;
}


bool isCollidingWithAnyAsteroid(float x, float y, float z, float scale) {
	float astronautRadius = 0.9;

	for (int i = 0; i < 3; i++) {
		if (isMoonTexture) {

			if (isColliding(x, y, z, astronautRadius, asteroidPosX[i], asteroidPosY[i], asteroidPosZ[i], asteroidRadius[i])) {
				PlaySound(TEXT("sound/boom.wav"), NULL, SND_FILENAME | SND_ASYNC);
				return true;

			}

		}
		else {

			if (isColliding(x, y, z, astronautRadius, asteroidPosX[i], asteroidPosY[i], asteroidPosZ[i], asteroidRadius[i])) {
				PlaySound(TEXT("sound/Thunder.wav"), NULL, SND_FILENAME | SND_ASYNC);
				return true;
			}
		}
	}
	return false;
}

void mySpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		cameraPosZ -= 0.5f;
		break;
	case GLUT_KEY_DOWN:
		cameraPosZ += 0.5f;
		break;
	case GLUT_KEY_LEFT:
		cameraPosX -= 0.5f;
		break;
	case GLUT_KEY_RIGHT:
		cameraPosX += 0.5f;
		break;
	case GLUT_KEY_F1:
		cameraTargetX -= 0.5f;
		break;
	case GLUT_KEY_F2:
		cameraTargetX += 0.5f;
		break;
	case GLUT_KEY_F3:
		cameraTargetY -= 0.5f;
		break;
	case GLUT_KEY_F4:
		cameraTargetY += 0.5f;
	case GLUT_KEY_F9:
		cameraTargetZ -= 0.5f;
		break;
	case GLUT_KEY_F10:
		cameraTargetZ += 0.5f;
		break;
	default:
		break;
	}

	printf("Camera Position: (%.2f, %.2f, %.2f)\n", cameraPosX, cameraPosY, cameraPosZ);
	printf("Camera Target: (%.2f, %.2f, %.2f)\n", cameraTargetX, cameraTargetY, cameraTargetZ);


	Eye.x = cameraPosX;
	Eye.y = cameraPosY;
	Eye.z = cameraPosZ;

	At.x = cameraTargetX;
	At.y = cameraTargetY;
	At.z = cameraTargetZ;

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	glutPostRedisplay();
}


void myKeyboard(unsigned char button, int x, int y)
{
	float newPosX = astronautPosX;
	float newPosY = astronautPosY;
	float newPosZ = astronautPosZ;
	float newCameraPosX = cameraPosX;
	float newCameraPosZ = cameraPosZ;
	float newCameraTargetX = cameraTargetX;
	float newCameraTargetZ = cameraTargetZ;


	switch (button)
	{
	case 'w':
		newPosZ -= 0.5f;
		astronautRotation = 0.0f;
		newCameraPosZ -= 0.5f;
		newCameraTargetZ -= 0.5f;
		if (isFirstPersonView) {
			if (astronautRotation == 0) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY + 1.5f;
				cameraPosZ = astronautPosZ - 1.5f;
				cameraTargetX = astronautPosX;
				cameraTargetY = astronautPosY - 1.0f;
				cameraTargetZ = astronautPosZ - 22.5f;
			}
			else if (astronautRotation == 90) {
				cameraPosX = astronautPosX - 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ;
				cameraTargetX = astronautPosX - 25.50;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ - 0.50;
			}
			else if (astronautRotation == 180) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 1.50;
				cameraTargetX = astronautPosX + 0.00;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ + 21.00;
			}
			else if (astronautRotation == 270) {
				cameraPosX = astronautPosX + 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 0.00;
				cameraTargetX = astronautPosX + 19.00;
				cameraTargetY = astronautPosY - 3.0f + 2.50;
				cameraTargetZ = astronautPosZ + 0.00;
			}
			newCameraPosX = cameraPosX;
			newCameraPosZ = cameraPosZ;
			newCameraTargetX = cameraTargetX;
			newCameraTargetZ = cameraTargetZ;
		}
		break;
	case 's':
		newPosZ += 0.5f;
		astronautRotation = 180.0f;
		newCameraPosZ += 0.5f;
		newCameraTargetZ += 0.5f;
		if (isFirstPersonView) {
			if (astronautRotation == 0) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.5f;
				cameraPosZ = astronautPosZ - 1.0f;
				cameraTargetX = astronautPosX;
				cameraTargetY = astronautPosY - 3.0f + 2.0f;
				cameraTargetZ = astronautPosZ - 22.5f;
			}
			else if (astronautRotation == 90) {
				cameraPosX = astronautPosX - 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ;
				cameraTargetX = astronautPosX - 25.50;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ - 0.50;
			}
			else if (astronautRotation == 180) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 1.50;
				cameraTargetX = astronautPosX + 0.00;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ + 21.00;
			}
			else if (astronautRotation == 270) {
				cameraPosX = astronautPosX + 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 0.00;
				cameraTargetX = astronautPosX + 19.00;
				cameraTargetY = astronautPosY - 3.0f + 2.50;
				cameraTargetZ = astronautPosZ + 0.00;
			}
			newCameraPosX = cameraPosX;
			newCameraPosZ = cameraPosZ;
			newCameraTargetX = cameraTargetX;
			newCameraTargetZ = cameraTargetZ;
		}
		break;
	case 'a':
		newPosX -= 0.5f;
		astronautRotation = 90.0f;
		newCameraPosX -= 0.5f;
		newCameraTargetX -= 0.5f;
		if (isFirstPersonView) {
			if (astronautRotation == 0) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.5f;
				cameraPosZ = astronautPosZ - 1.0f;
				cameraTargetX = astronautPosX;
				cameraTargetY = astronautPosY - 3.0f + 2.0f;
				cameraTargetZ = astronautPosZ - 22.5f;
			}
			else if (astronautRotation == 90) {
				cameraPosX = astronautPosX - 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ;
				cameraTargetX = astronautPosX - 25.50;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ - 0.50;
			}
			else if (astronautRotation == 180) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 1.50;
				cameraTargetX = astronautPosX + 0.00;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ + 21.00;
			}
			else if (astronautRotation == 270) {
				cameraPosX = astronautPosX + 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 0.00;
				cameraTargetX = astronautPosX + 19.00;
				cameraTargetY = astronautPosY - 3.0f + 2.50;
				cameraTargetZ = astronautPosZ + 0.00;
			}
			newCameraPosX = cameraPosX;
			newCameraPosZ = cameraPosZ;
			newCameraTargetX = cameraTargetX;
			newCameraTargetZ = cameraTargetZ;
		}
		break;
	case 'd':
		newPosX += 0.5f;
		astronautRotation = 270.0f;
		newCameraPosX += 0.5f;
		newCameraTargetX += 0.5f;
		if (isFirstPersonView) {
			if (astronautRotation == 0) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.5f;
				cameraPosZ = astronautPosZ - 1.0f;
				cameraTargetX = astronautPosX;
				cameraTargetY = astronautPosY - 3.0f + 2.0f;
				cameraTargetZ = astronautPosZ - 22.5f;
			}
			else if (astronautRotation == 90) {
				cameraPosX = astronautPosX - 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ;
				cameraTargetX = astronautPosX - 25.50;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ - 0.50;
			}
			else if (astronautRotation == 180) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 1.50;
				cameraTargetX = astronautPosX + 0.00;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ + 21.00;
			}
			else if (astronautRotation == 270) {
				cameraPosX = astronautPosX + 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 0.00;
				cameraTargetX = astronautPosX + 19.00;
				cameraTargetY = astronautPosY - 3.0f + 2.50;
				cameraTargetZ = astronautPosZ + 0.00;
			}
			newCameraPosX = cameraPosX;
			newCameraPosZ = cameraPosZ;
			newCameraTargetX = cameraTargetX;
			newCameraTargetZ = cameraTargetZ;
		}

		break;
	case 'z':
		cameraPosY += 0.5f;
		break;
	case 'x':
		cameraPosY -= 0.5f;
		break;
	case 'J':
		isFirstPersonView = !isFirstPersonView;
		if (isFirstPersonView) {
			if (astronautRotation == 0) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.5f;
				cameraPosZ = astronautPosZ - 1.0f;
				cameraTargetX = astronautPosX;
				cameraTargetY = astronautPosY - 3.0f + 2.0f;
				cameraTargetZ = astronautPosZ - 22.5f;
			}
			else if (astronautRotation == 90) {
				cameraPosX = astronautPosX - 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ;
				cameraTargetX = astronautPosX - 25.50;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ - 0.50;
			}
			else if (astronautRotation == 180) {
				cameraPosX = astronautPosX;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 1.50;
				cameraTargetX = astronautPosX + 0.00;
				cameraTargetY = astronautPosY - 3.0f + 2.00;
				cameraTargetZ = astronautPosZ + 21.00;
			}
			else if (astronautRotation == 270) {
				cameraPosX = astronautPosX + 1.50;
				cameraPosY = astronautPosY - 3.0f + 4.50;
				cameraPosZ = astronautPosZ + 0.00;
				cameraTargetX = astronautPosX + 19.00;
				cameraTargetY = astronautPosY - 3.0f + 2.50;
				cameraTargetZ = astronautPosZ + 0.00;
			}
		}
		else {
			cameraPosX = astronautPosX;
			cameraPosY = astronautPosY + 10.0f;
			cameraPosZ = astronautPosZ + 8.5f;
			cameraTargetX = astronautPosX;
			cameraTargetY = astronautPosY;
			cameraTargetZ = astronautPosZ;
		}

		newCameraPosX = cameraPosX;
		newCameraPosZ = cameraPosZ;
		newCameraTargetX = cameraTargetX;
		newCameraTargetZ = cameraTargetZ;

		break;
	case 'i':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'o':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	float minX = -15.0f;
	float maxX = 15.0f;
	float minZ = -15.0f;
	float maxZ = 15.0f;

	if (newPosX < minX || newPosX > maxX || newPosZ < minZ || newPosZ > maxZ) {
		return;
	}

	if (!isCollidingWithAnyAsteroid(newPosX, 3.0, newPosZ, 0.9)) {
		astronautPosX = newPosX;
		astronautPosY = 3.0;
		astronautPosZ = newPosZ;
		cameraPosX = newCameraPosX;
		cameraPosZ = newCameraPosZ;
		cameraTargetX = newCameraTargetX;
		cameraTargetZ = newCameraTargetZ;

	}


	Eye.x = cameraPosX;
	Eye.y = cameraPosY;
	Eye.z = cameraPosZ;

	At.x = cameraTargetX;
	At.y = cameraTargetY;
	At.z = cameraTargetZ;
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	glutPostRedisplay();


	printf("Camera Position: X = %.2f, Y = %.2f, Z = %.2f\n", crystalPosX[1], crystalPosY[1], crystalPosZ[1]);
	printf("Camera Target: X = %.2f, Y = %.2f, Z = %.2f\n", newPosX, newPosY, newPosZ);

	if (!isCollidingWithAnyAsteroid(newPosX, 3.0, newPosZ, 0.9)) {
		astronautPosX = newPosX;
		astronautPosY = 3.0;
		astronautPosZ = newPosZ;

	}
	if (level1 && isMoonTexture) {
		float epsilon = 1.4f;
		if (abs(newPosX - (10.00)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - 10.00) < epsilon) {
			if (!crystalCollected[2]) {
				score += 10;
				crystalCollected[2] = true;
				PlaySound(TEXT("sound/Diamond.wav"), NULL, SND_FILENAME | SND_ASYNC);

			}
			crystalPosX[2] = 100.0f;
			crystalPosY[2] = 100.0f;
			crystalPosZ[2] = 100.0f;
		}

		if (abs(newPosX - (-3.00)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - (-9.00)) < epsilon) {
			if (!crystalCollected[0]) {
				score += 10;
				crystalCollected[0] = true;
				PlaySound(TEXT("sound/Diamond.wav"), NULL, SND_FILENAME | SND_ASYNC);

			}
			crystalPosX[0] = 100.0f;
			crystalPosY[0] = 100.0f;
			crystalPosZ[0] = 100.0f;
		}

		if (abs(newPosX - (-10.00)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - (10.00)) < epsilon) {
			if (!crystalCollected[1]) {
				score += 10;
				crystalCollected[1] = true;
				PlaySound(TEXT("sound/Diamond.wav"), NULL, SND_FILENAME | SND_ASYNC);

			}
			crystalPosX[1] = 100.0f;
			crystalPosY[1] = 100.0f;
			crystalPosZ[1] = 100.0f;
		}

	}
	else {
		float epsilon = 1.4f;

		if (abs(newPosX - (14.00)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - (-10.60)) < epsilon) {
			if (!tresureCollected[1]) {
				score += 10;
				tresureCollected[1] = true;
			}
			tresurePosX[1] = 100.0f;
			tresurePosY[1] = 100.0f;
			tresurePosZ[1] = 100.0f;
		}

		if (abs(newPosX - (-4.00)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - (-3.20)) < epsilon) {
			if (!tresureCollected[0]) {
				score += 10;
				tresureCollected[0] = true;
			}
			tresurePosX[0] = 100.0f;
			tresurePosY[0] = 100.0f;
			tresurePosZ[0] = 100.0f;
		}

		if (abs(newPosX - (8.0)) < epsilon && abs(newPosY - 3.00) < epsilon && abs(newPosZ - (14.40)) < epsilon) {
			if (!tresureCollected[2]) {
				score += 10;
				tresureCollected[2] = true;
			}
			tresurePosX[2] = 100.0f;
			tresurePosY[2] = 100.0f;
			tresurePosZ[2] = 100.0f;
		}


	}



	glutPostRedisplay();
}



//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float epsilon = 1.4f;
		if (abs(astronautPosZ - (-15.40)) < epsilon && abs(astronautPosY - 3.00) < epsilon && abs(astronautPosX - 2.00) < epsilon
			&& level1 && isMoonTexture) {
			level1 = false;
			isMoonTexture = false;
			PlaySound(TEXT("sound/wormhole.wav"), NULL, SND_FILENAME | SND_ASYNC);

		}
		else if (abs(astronautPosZ - (-15.40)) < epsilon && abs(astronautPosY - 3.00) < epsilon && abs(astronautPosX - 6.00) < epsilon
			&& !level1 && !isMoonTexture)
		{
			PlaySound(TEXT("sound/electricportal.wav"), NULL, SND_FILENAME | SND_ASYNC);
			won = true;

		}
		else {
			astronautVelY = 0.2f;
		}
	}
	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
		if (button == GLUT_LEFT_BUTTON) {
			astronautVelY = 5.0f;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		myKeyboard('J', x, y);
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	model_Crystal.Load("Models/Crystal/Crystal.3ds");
	model_WarmHole.Load("Models/WarmHole/model.3ds");
	model_astronaut.Load("Models/Astronaut/Spaceman N100519.3ds");
	model_Asteriod.Load("Models/Asteriod/ASTHANGR.3ds");
	model_NebulaCloud.Load("Models/Nebula Cloud2/model.3ds");
	model_Spacetreasure.Load("Models/Space Tresaure/model.3ds");
	model_Portal.Load("Models/Portal/model.3ds");

	tex_ground.Load("Textures/download.bmp");
	tex_jupiter.Load("Textures/Jupiter.bmp");

	if (level1 && isMoonTexture) {
		loadBMP(&tex, "Textures/download (1).bmp", true);
	}
	else if (!level1 && !isMoonTexture) {
		loadBMP(&tex2, "Textures/Space1.bmp", true);
	}

}


//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	cameraPosX = astronautPosX;
	cameraPosY = astronautPosY + 10.0f;
	cameraPosZ = astronautPosZ + 8.5f;
	Eye.x = cameraPosX;
	Eye.y = cameraPosY;
	Eye.z = cameraPosZ;


	cameraTargetX = astronautPosX;
	cameraTargetY = astronautPosY;
	cameraTargetZ = astronautPosZ;
	At.x = cameraTargetX;
	At.y = cameraTargetY;
	At.z = cameraTargetZ;



	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1280, 720);

	glutInitWindowPosition(0, 0);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	//glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutIdleFunc(Anim);

	glutSpecialFunc(mySpecialKeyboard);
	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	WCHAR music2[] = L"sound/Astronautintheocean.wav";
	LPCSTR const Back_ground_music2 = "open sound/Astronautintheocean.wav type mpegvideo alias Sound";
	mciSendStringA(Back_ground_music2, NULL, 0, NULL);
	MCIERROR onrepeat2 = mciSendStringA("play Sound repeat", NULL, 0, NULL);
	glutMainLoop();
}