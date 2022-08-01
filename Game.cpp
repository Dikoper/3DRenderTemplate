#include "DebugHeader.h"
#include "Engine.h"
#include "DrawExt.h"
#include <stdlib.h>
#include <iostream>
#include <memory.h>

int z_buffer[SCREEN_HEIGHT][SCREEN_WIDTH] = { 0 };

float time_cnt;
float view_zoom, view_angle;

Matrix4x4 modelMTRX, viewMTRX, projectionMTRX, deviceMTRX, mvp;
Model3D planeModel, objectModel;

void calculateModel();
void calculateCam();
void calculateView();

void initialize()
{
	time_cnt = 0.f;
	view_zoom = 0.5f;
	view_angle = 0.f;

	planeModel.loadFromFile("./models/plane.obj");
	planeModel.loadTexture("./models/full/floor_diffuse.tga", planeModel.diffuse_tx);

	if (!objectModel.loadFromFile("./models/full/african_head/african_head.obj"))
		std::cerr << "Cant load obj";
	if (!objectModel.loadTexture("./models/full/african_head/african_head_diffuse.tga", objectModel.diffuse_tx))
		std::cerr << "Cant load texture";
	if (!objectModel.loadTexture("./models/full/african_head/african_head_nm.tga", objectModel.normalmap_tx))
		std::cerr << "Cant load texture";
	if (!objectModel.loadTexture("./models/full/african_head/african_head_spec.tga", objectModel.specularmap_tx))
		std::cerr << "Cant load texture";
	
	calculateViewport();
}

void act(float dt)
{
	calculateModel();
	calculateCam();
	calculateView();

	mvp = modelMTRX * viewMTRX * projectionMTRX * deviceMTRX; //move to distinct class

	if (is_key_pressed(VK_UP))
		view_zoom += 0.5f * dt;
	if (is_key_pressed(VK_DOWN))
		view_zoom -= 0.5f * dt;
	if (is_key_pressed(VK_LEFT))
		cam_pos.x -= 1.5f * dt;
	if (is_key_pressed(VK_RIGHT))
		cam_pos.x += 1.5f * dt;
	if (is_key_pressed(VK_TAB))
		debug_draw = !debug_draw;
	if (is_key_pressed(VK_CTRL))
		isScanline = !isScanline;
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
	if (is_key_pressed(VK_SPACE))
		time_cnt = 0;
	else
		time_cnt += dt;
}

void draw()
{
	// clear backbuffer
	memset(buffer, 128, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
	// clear zbuffer
	memset(z_buffer, std::numeric_limits<int>::min(), SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(int));

	m_Model = modelMTRX; m_View = viewMTRX;
	drawFilledModel(planeModel, shader_sh, cam_pos, light_pos, CLR_GREEN);
	drawFilledModel(objectModel, bumped_sh, cam_pos, light_pos, CLR_RED);
	//drawZBuff();
	//drawWireframeModel(objectModel, CLR_GREEN);
}

void calculateCam() 
{
	Matrix4x4 _rot = { new float[16]{
		cosf(-view_angle),0,-sinf(-view_angle),0,
		0,1,0,0,
		sinf(-view_angle),0,cosf(-view_angle),0,
		0,0,0,1}
	};
}

void calculateView()
{
	float angle = -0.0f;//slightly broken

	Matrix4x4 m_rot = { new float[16]{
		1,0,0,0,
		0,cosf(angle),-sinf(angle),0,
		0,sinf(angle),cosf(angle),0,
		0,0,0,1}
	};

	Matrix4x4 m_scale = { new float[16]{
		view_zoom,0,0,0,
		0,view_zoom,0,0,
		0,0,view_zoom,0,
		0,0,0,1}
	};

	Matrix4x4 m_trans = { new float[16]{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,10,1}
	};

	viewMTRX = m_rot * m_trans * m_scale;
}

void calculateModel()
{
	modelMTRX = { new float[16]{
		cosf(time_cnt),0,-sinf(time_cnt),0,
		0,1,0,0,
		sinf(time_cnt),0,cosf(time_cnt),0,
		0,0,0,1}
	};
}

void finalize()
{
}