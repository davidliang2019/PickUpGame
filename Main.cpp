//
//	Main.cpp
//
// This program is written by Lingfeng Liang in Class CS809
// The ObjLibrary is written by Dr. Howard J. Hamilton
// Keys 'w', 's', 'a', 'd' control the move
// Keys 'up', 'down', 'left', 'right' control the direction
// Key 'space' jumps and key 'r' reset position

#include <sstream>
#include "Sleep.h"
#include "World.h"
#include "Player.h"
#include "DeltaTime.h"
#include "PickupManager.h"
#include "ObjLibrary/SpriteFont.h"
#include "ObjLibrary/ObjModel.h"
#include "Enemy.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	const double FRAME_LENGTH = 1.0 / 60.0;
	const double  PLAYER_MOVE_RATE = 25.0;  // meters per second
	const double  PLAYER_STRAFE_RATE = 10.0;  // meters per second
	const double  PLAYER_TURN_RATE = 2.0;  // radians per second
	const float PLAYER_RADIUS = 0.25f;
	const float PLAYER_HALF_HEIGHT = 0.8f;
	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 480;

	const unsigned int KEY_UP_ARROW = 256;
	const unsigned int KEY_DOWN_ARROW = 257;
	const unsigned int KEY_LEFT_ARROW = 258;
	const unsigned int KEY_RIGHT_ARROW = 259;
	const unsigned int KEY_COUNT = 260;

	bool key_pressed[KEY_COUNT];
	ObjModel skybox;			// create a skybox object
	DisplayList skybox_list;
	World g_world;			// create a world object
	PickupManager g_pickup;
	Player g_player;
	SpriteFont g_font;
	Enemy g_enemy;
}

// this function clear the screen
void initDisplay ()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

// this function load the model objects
void init(string file_name)
{
	for (int i = 0; i < KEY_COUNT; i++)
		key_pressed[i] = false;
	initDisplay();

	skybox.load("Models/Skybox.obj");	// load the skybox object 
	skybox_list = skybox.getDisplayList();

	g_world.init(file_name);		// initial the world object
	g_player.init(g_world);
	g_pickup.init(g_world);
	g_enemy.init(g_world);

	g_font.load("Font.bmp");

	DeltaTime::start_time = glutGet(GLUT_ELAPSED_TIME);
	DeltaTime::previous_time = DeltaTime::start_time;
}

// this function handles main keyboad input
void keyboardDown (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	case 'r':
		if (!key_pressed['r'])
			g_player.init(g_world);
		break;
	}

	if (key >= 'A' && key <= 'Z')
		key = key - 'A' + 'a';
	key_pressed[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
	if (key >= 'A' && key <= 'Z')
		key = key - 'A' + 'a';
	key_pressed[key] = false;
}

// this function handles special keyboad input
void specialDown(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = true;
		break;
	}
}

void specialUp(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = false;
		break;
	}
}

void handleKeybroad()
{
	if (key_pressed[KEY_LEFT_ARROW])
		g_player.turnLeft();
	if (key_pressed[KEY_RIGHT_ARROW])
		g_player.turnRight();
	if (key_pressed['w'] || key_pressed[KEY_UP_ARROW])
		g_player.moveForward(g_world);
	if (key_pressed['s'] || key_pressed[KEY_DOWN_ARROW])
		g_player.moveBackward(g_world);
	if (key_pressed['a'])
		g_player.moveLeft(g_world);
	if (key_pressed['d'])
		g_player.moveRight(g_world);
	if (key_pressed[' '])
		g_player.jump();
}


// this function update all positions
void update ()
{
	DeltaTime::current_time = glutGet(GLUT_ELAPSED_TIME);
	DeltaTime::elapsed_time = DeltaTime::current_time - DeltaTime::start_time;
	DeltaTime::update_lag += (float)(DeltaTime::current_time - DeltaTime::previous_time);
	if (DeltaTime::update_lag > DeltaTime::FIXED_DELTA_TIME * 1000.0) {
		while (DeltaTime::update_lag > DeltaTime::FIXED_DELTA_TIME * 1000.0) {
			handleKeybroad();
			g_player.update(g_world);
			g_pickup.update();
			g_pickup.pickingUp(g_player.getPosition(), PLAYER_RADIUS, PLAYER_HALF_HEIGHT);
			g_enemy.update(g_player);

			if (g_enemy.isMoreHit())
				g_player.isAttacked(g_enemy.getHitVelocity());

			DeltaTime::physics_update++;
			DeltaTime::update_lag -= DeltaTime::FIXED_DELTA_TIME * 1000.0f;
		}
		glutPostRedisplay();
	}
	else {
		sleep(DeltaTime::FIXED_DELTA_TIME - DeltaTime::update_lag / 1000.0f);
	}
	DeltaTime::previous_time = DeltaTime::current_time;
}

// this function reshape the world update
void reshape (int w, int h)
{
	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void drawSkybox()
{
	glPushMatrix();
	glTranslated(g_player.getPosition().x, 
		g_player.getPosition().y, 
		g_player.getPosition().z);
	glScalef(600.0f, 600.0f, 600.0f);
	glDepthMask(GL_FALSE);
	skybox_list.draw();
	glDepthMask(GL_TRUE);
	glPopMatrix();
}

void drawOverlays()
{
	stringstream score_ss;
	if (DeltaTime::elapsed_time != 0)
		score_ss << "Physics Update: "
		<< (float)DeltaTime::physics_update / DeltaTime::elapsed_time * 1000 << endl
		<< "Frame Rate: "
		<< (float)DeltaTime::total_frames / DeltaTime::elapsed_time * 1000 << endl;
	score_ss << "Score: " << g_pickup.getScore();

	SpriteFont::setUp2dView(WINDOW_WIDTH, WINDOW_HEIGHT);
	g_font.draw(score_ss.str(), WINDOW_WIDTH - 260, 16);
	SpriteFont::unsetUp2dView();
}

/*void drawBat()
{
	for (int i = 0; i < g_world.getDiskCount(); i++) {
		if(!m_bats[i].isDead())
			m_bats[i].draw();
	}
}*/

//this function display all objects
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display at all

	DeltaTime::total_frames++;

	glLoadIdentity();

	if (key_pressed['o']) {	// switch the camera to overview camera by 'o' key
		gluLookAt(100.0 + g_player.getPosition().x, 200.0, 10.0 + g_player.getPosition().z,  // (x, y, z) camera eye position
			0.0 + g_player.getPosition().x, 0.8, 0.0 + g_player.getPosition().z,  // (x, y, z) camera look at position
			0.0, 1.0, 0.0); // (x, y, z) camera up direction
		g_pickup.drawMovement();
	}
	else {
		gluLookAt(g_player.getPosition().x - 2.0 * g_player.getForward().x,
			g_player.getPosition().y + 0.4,
			g_player.getPosition().z - 2.0 * g_player.getForward().z,  // (x, y, z) camera eye position
			g_player.getPosition().x, g_player.getPosition().y, g_player.getPosition().z,  // (x, y, z) camera look at position
			0.0, 1.0, 0.0); // (x, y, z) camera up direction
		drawSkybox();		// draw a skybox
	}

	// camera is now set up - any drawing before here will display incorrectly
	// draw all world disks
	g_world.draw();

	g_pickup.draw();
	g_enemy.draw();
	//drawBat();

	// draw a character model
	g_player.draw();

	if (!key_pressed['o'])
		drawOverlays();  // must be drawn last

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Pick Up Game");
	glutKeyboardFunc(&keyboardDown);
	glutKeyboardUpFunc(&keyboardUp);
	glutSpecialFunc(&specialDown);
	glutSpecialUpFunc(&specialUp);
	glutIdleFunc(&update);
	glutReshapeFunc(&reshape);
	glutDisplayFunc(&display);

	init("Worlds/Basic.txt");
	//init("Worlds/Dense.txt");
	//init("Worlds/Icy.txt");
	//init("Worlds/Leafy.txt");
	//init("Worlds/Rocky.txt");
	//init("Worlds/Sandy.txt");
	//init("Worlds/Simple.txt");
	//init("Worlds/Small.txt");
	//init("Worlds/Sparse.txt");
	//init("Worlds/Twisted.txt");

	glutMainLoop();  // contains an infinite loop, so it never returns
	return 1;
}