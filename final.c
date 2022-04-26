/*
* Kelley Kelley
* Final Project
* CSCI 4239/5239 Spring 2022

Final Project for Advanced Computer Graphics

A lot of spheres that collide with each other and do gravity.
It took way too long, but it looks really freaking cool
move around, watch some stars collide, hit m a few times to see some
other cool stars collide

Key bindings:
esc - quit
0 - reset view
r - reset particles
arrows - change view angle
pgup/pgdn - zoom in and out
m - change mode
space - pause/play simulation
wasdeq - move camera around
*/
#include "mylib.h"
double dim = 80; // size of the world
int th = 0; // azimuth of view angle
int ph = 0; // elevation of view angle
int fov = 57;
int nw, ng; // work group size and count
int maxnw = 1024, maxng = 4; // need to control amount bc we do a lot of math
// so don't wanna just maximize this bc that's too much math
int n; // number of particles
double asp = 1; // aspect ratio
int computeshader; // compute shader program
int colorshader; // shader program
unsigned int posbuf1; // position buffers
unsigned int posbuf2;
unsigned int velbuf1; // velocity buffers
unsigned int velbuf2;
unsigned int colbuf; // color buffer
int buf; // buffer value
int mode = 4; // start in mode 4 because that one is my absolute favorite
int maxmodes = 8;
double camx = 0; // camera position
double camy = 0;
double camz = 0;

bool movement = true; // for testing, hit space and pauses physics which is nice

typedef struct {
	union { float x; float r; };
	union { float y; float g; };
	union { float z; float b; };
	union { float w; float a; };
} vec4;

// pretty self explanatory
// an icosahedrons data for instancing
const float icosahedron_data[] = {
	0.894, 0.000, 0.447, 1,		0.724, 0.526,-0.447, 1,		0.724,-0.526,-0.447, 1,
	0.276,-0.851, 0.447, 1,		0.724,-0.526,-0.447, 1,		-0.276,-0.851,-0.447, 1,
	-0.724,-0.526, 0.447, 1,	-0.276,-0.851,-0.447, 1,	-0.894, 0.000,-0.447, 1,
	-0.724, 0.526, 0.447, 1,	-0.894, 0.000,-0.447, 1,	-0.276, 0.851,-0.447, 1,
	0.276, 0.851, 0.447, 1,		-0.276, 0.851,-0.447, 1,	0.724, 0.526,-0.447, 1,
	0.276,-0.851, 0.447, 1,		0.894, 0.000, 0.447, 1,		0.724,-0.526,-0.447, 1,
	-0.724,-0.526, 0.447, 1,	0.276,-0.851, 0.447, 1,		-0.276,-0.851,-0.447, 1,
	-0.724, 0.526, 0.447, 1,	-0.724,-0.526, 0.447, 1,	-0.894, 0.000,-0.447, 1,
	 0.276, 0.851, 0.447, 1,	-0.724, 0.526, 0.447, 1,	-0.276, 0.851,-0.447, 1,
	0.894, 0.000, 0.447, 1,		0.276, 0.851, 0.447, 1,		0.724, 0.526,-0.447, 1,
	0.000, 0.000,-1.000, 1,		0.724,-0.526,-0.447, 1,		0.724, 0.526,-0.447, 1,
	0.000, 0.000,-1.000, 1,		-0.276,-0.851,-0.447, 1,	0.724,-0.526,-0.447, 1,
	0.000, 0.000,-1.000, 1,		-0.894, 0.000,-0.447, 1,	-0.276,-0.851,-0.447, 1,
	0.000, 0.000,-1.000, 1,		-0.276, 0.851,-0.447, 1,	-0.894, 0.000,-0.447, 1,
	0.000, 0.000,-1.000, 1,		0.724, 0.526,-0.447, 1,		-0.276, 0.851,-0.447, 1,
	0.894, 0.000, 0.447, 1,		0.276,-0.851, 0.447, 1,		0.000, 0.000, 1.000, 1,
	0.276, 0.851, 0.447, 1,		0.894, 0.000, 0.447, 1,		0.000, 0.000, 1.000, 1,
	-0.724, 0.526, 0.447, 1,	0.276, 0.851, 0.447, 1,		0.000, 0.000, 1.000, 1,
	-0.724,-0.526, 0.447, 1,	-0.724, 0.526, 0.447, 1,	0.000, 0.000, 1.000, 1,
	0.276,-0.851, 0.447, 1,		-0.724,-0.526, 0.447, 1,	0.000, 0.000, 1.000, 1
};
unsigned int icoVBO;
unsigned int icoVAO;
unsigned int icoTriangles = 20;
unsigned int icoVertices = 60;
unsigned int icoSize = 240;

// reset particles
void ResetParticles() {
	vec4 *pos1, *pos2, *vel1, *vel2, *col;
	// reset position
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf1);
	pos1 = (vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	// based on our mode initialize our positions differently
	for (int i = 0; i < n; i++) {
		switch (mode) {
		case 0:
			if (i % 10 == 0) {
				pos1[i].x = i * 3 - 50;
				pos1[i].y = -20;
				pos1[i].z = 0;
				pos1[i].w = 1;
			}
			else {
				pos1[i].x = i * 3 - 200;
				pos1[i].y = 20;
				pos1[i].z = 0;
				pos1[i].w = 1;
			}
			break;
		case 1:
			if (i % 10 == 0) {
				pos1[i].x = i * 3 - 50;
				pos1[i].y = -20;
				pos1[i].z = 0;
				pos1[i].w = 1;
			}
			else {
				pos1[i].x = i * 3 - 211;
				pos1[i].y = 20;
				pos1[i].z = 0;
				pos1[i].w = 1;
			}
			break;
		case 2:
			pos1[i].x = frand(-100, 100);
			pos1[i].y = frand(-100, 100);
			pos1[i].z = frand(-100, 100);
			pos1[i].w = 1;
			break;
		case 3:
			if (i % 2 == 0) {
				pos1[i].x = frand(-100, 25);
				pos1[i].y = frand(-100, 25);
				pos1[i].z = frand(-100, 25);
				pos1[i].w = 1;
			}
			else {
				pos1[i].x = frand(75, 25);
				pos1[i].y = frand(75, 25);
				pos1[i].z = frand(75, 25);
				pos1[i].w = 1;
			}
			break;
		case 4:
			if (i % 4 == 0) {
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
			}
			else if (i % 4 == 1) {
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
			}
			else if (i % 4 == 2) {
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
			}
			else {
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
			}
			break;
		case 5:
			switch (i % 8) {
			case 0:
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
				break;
			case 1:
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
				break;
			case 2:
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
				break;
			case 3:
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
				break;
			case 4:
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
				break;
			case 5:
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(75, 125);
				pos1[i].w = 1;
				break;
			case 6:
				pos1[i].x = frand(75, 125);
				pos1[i].y = frand(75, 125);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
				break;
			case 7:
				pos1[i].x = frand(-125, -75);
				pos1[i].y = frand(-125, -75);
				pos1[i].z = frand(-125, -75);
				pos1[i].w = 1;
				break;
			}
			break;
		case 6:
			switch (i % 8) {
			case 0:
				pos1[i].x = frand(25, 50);
				pos1[i].y = frand(25, 50);
				pos1[i].z = frand(25, 50);
				pos1[i].w = 1;
				break;
			case 1:
				pos1[i].x = frand(-50, -25);
				pos1[i].y = frand(25, 50);
				pos1[i].z = frand(25, 50);
				pos1[i].w = 1;
				break;
			case 2:
				pos1[i].x = frand(-50, -25);
				pos1[i].y = frand(-50, -25);
				pos1[i].z = frand(25, 50);
				pos1[i].w = 1;
				break;
			case 3:
				pos1[i].x = frand(-50, -25);
				pos1[i].y = frand(25, 50);
				pos1[i].z = frand(-50, -25);
				pos1[i].w = 1;
				break;
			case 4:
				pos1[i].x = frand(25, 50);
				pos1[i].y = frand(-50, -25);
				pos1[i].z = frand(-50, -25);
				pos1[i].w = 1;
				break;
			case 5:
				pos1[i].x = frand(25, 50);
				pos1[i].y = frand(-50, -25);
				pos1[i].z = frand(25, 50);
				pos1[i].w = 1;
				break;
			case 6:
				pos1[i].x = frand(25, 50);
				pos1[i].y = frand(25, 50);
				pos1[i].z = frand(-50, -25);
				pos1[i].w = 1;
				break;
			case 7:
				pos1[i].x = frand(-50, -25);
				pos1[i].y = frand(-50, -25);
				pos1[i].z = frand(-50, -25);
				pos1[i].w = 1;
				break;
			}
			break;
		case 7:
			switch (i % 4) {
			case 0:

				break;
			case 1:

				break;
			case 2:

				break;
			case 3:

				break;
			}
			break;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// set our second position buffer to be exactly the same as the first
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf2);
	pos2 = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++) {
		pos2[i].x = pos1[i].x;
		pos2[i].y = pos1[i].y;
		pos2[i].z = pos1[i].z;
		pos2[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset velocities
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf1);
	vel1 = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	// depending on mode, do velocities differently
	for (int i = 0; i < n; i++)
	{
		switch (mode) {
		case 0:
		case 1:
			if (i % 10 == 0) {
				vel1[i].x = 0;
				vel1[i].y = 0;
				vel1[i].z = 0;
				vel1[i].w = 0;
			}
			else {
				vel1[i].x = 0;
				vel1[i].y = -8;
				vel1[i].z = 0;
				vel1[i].w = 0;
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			vel1[i].x = frand(-2, 2);
			vel1[i].y = frand(-2, 2);
			vel1[i].z = frand(-2, 2);
			vel1[i].w = 0;
			break;
		case 7:
			switch (i % 4) {
			case 0:

				break;
			case 1:

				break;
			case 2:

				break;
			case 3:

				break;
			break;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// copy velocity 1 into velocity 2
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf2);
	vel2 = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		vel2[i].x = vel1[i].x;
		vel2[i].y = vel1[i].y;
		vel2[i].z = vel1[i].z;
		vel2[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset colors
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colbuf);
	col = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		col[i].r = frand(0.1, .2);
		col[i].g = frand(0.1, .2);
		col[i].b = frand(0.4, .2);
		col[i].a = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Set buffer base
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posbuf1);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, posbuf2);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, velbuf1);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, velbuf2);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, colbuf);

	//  Unset buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// init particles
void InitParticles() {
	// get max workgroup size and count
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &ng);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &nw);
	// clamp them, my simulation dies if there are too many particles
	if (ng > maxng) ng = maxng;
	if (nw > maxnw) nw = maxnw;
	n = nw * ng;

	// initialize position buffers
	glGenBuffers(1, &posbuf1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf1);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &posbuf2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);

	// initialize velocity buffer
	glGenBuffers(1, &velbuf1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf1);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &velbuf2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);

	// initialize color buffer
	glGenBuffers(1, &colbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// initialize ico stuff
	glGenBuffers(1, &icoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, icoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(icosahedron_data), icosahedron_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// intialize our vertices for the icosahedron
	glUseProgram(colorshader);
	glGenVertexArrays(1, &icoVAO);
	glBindVertexArray(icoVAO);
	glBindBuffer(GL_ARRAY_BUFFER, icoVBO);
	int loc = glGetAttribLocation(colorshader, "Vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);

	// intialize our translation arrays and attribute divisor them by one
	// so each icosahedron uses the same data to move it
	glBindBuffer(GL_ARRAY_BUFFER, posbuf1);
	loc = glGetAttribLocation(colorshader, "translation1");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);
	glVertexAttribDivisor(loc, 1);
	glBindBuffer(GL_ARRAY_BUFFER, posbuf2);
	loc = glGetAttribLocation(colorshader, "translation2");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);
	glVertexAttribDivisor(loc, 1);
	// same thing with color as translation
	glBindBuffer(GL_ARRAY_BUFFER, colbuf);
	loc = glGetAttribLocation(colorshader, "color");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);
	glVertexAttribDivisor(loc, 1);
	// reset everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// reset particle positions / this is the first time so kinda set them in the first place
	ResetParticles();
}

// draw the particles
void DrawParticles() {
	// set shader
	glUseProgram(colorshader);
	// eye stuff
	float Ex = -2 * dim * Sin(th) * Cos(ph) + camx;
	float Ey = +2 * dim * Sin(ph) + camy;
	float Ez = +2 * dim * Cos(th) * Cos(ph) + camz;

	// projection matrix
	float proj[16];
	mat4identity(proj);
	mat4perspective(proj, fov, asp, dim / 16, 16 * dim);
	// view matrix
	float view[16];
	mat4identity(view);
	mat4lookAt(view, Ex, Ey, Ez, camx, camy, camz, 0, Cos(ph), 0);
	// modelview matrix
	float modelview[16];
	mat4copy(modelview, view);
	mat4translate(modelview, 0, 0, 0.5);
	// pass to vertex shader
	int id = glGetUniformLocation(colorshader, "ProjectionMatrix");
	glUniformMatrix4fv(id, 1, 0, proj);
	id = glGetUniformLocation(colorshader, "ModelViewMatrix");
	glUniformMatrix4fv(id, 1, 0, modelview);
	id = glGetUniformLocation(colorshader, "buf");
	glUniform1i(id, buf);

	// draw the ico
	glBindVertexArray(icoVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, icoSize, n);
	glBindVertexArray(0);
	
	// return to default shader
	glUseProgram(0);
}

// run compute shader
void compute() {
	glUseProgram(computeshader);
	// rather than switch which one the compute shader is effecting which seems
	// dumb and much more complicated, just switch what we send to the compute shader
	if (buf) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posbuf1);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, posbuf2);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, velbuf1);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, velbuf2);
	}
	else {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, posbuf1);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posbuf2);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, velbuf1);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, velbuf2);
	}
	// switch which one is our base buffer for positions and velocities
	buf = (buf + 1) % 2;
	// tell compute shader how many particles we got
	int id = glGetUniformLocation(computeshader, "n");
	//printf("%i", n);
	glUniform1i(id, n);
	glDispatchCompute(ng, 1, 1);
	glUseProgram(0);
	// wait for compute shader
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// refresh display
void display(GLFWwindow* window) {
	// erase window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// set eye position
	View(th, ph, 55, dim);

	// compute shader
	// rather than do anything like actually pausing or some complicated nonsense, just don't 
	// run the compute shader which is where all the math happens
	if(movement)
		compute();

	// draw the particles
	DrawParticles();

	glDisable(GL_DEPTH_TEST);
	// display parameters
	glWindowPos2i(5, 5);
	Print("FPS=%d, work groups=%i, work size=%i, Count %i", FramesPerSecond(), ng, nw, n);
	// render
	ErrCheck("display");
	glFlush();
	glfwSwapBuffers(window);
}

// key pressed callback
void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// discard key releases (keeps PRESS and REPEAT)
	if (action == GLFW_RELEASE) return;

	// exit on esc
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, 1);
	// reset view
	else if (key == GLFW_KEY_0) {
		th = ph = 0;
		dim = 50;
		camx = camy = camz = 0;
	}
	// reset particles
	else if (key == GLFW_KEY_R)
		ResetParticles();
	// increase angle
	else if (key == GLFW_KEY_RIGHT)
		th += 5;
	// decrease angle
	else if (key == GLFW_KEY_LEFT)
		th -= 5;
	// increase elevation
	else if (key == GLFW_KEY_UP)
		ph += 5;
	// decrease elevation
	else if (key == GLFW_KEY_DOWN)
		ph -= 5;
	// page up
	else if (key == GLFW_KEY_PAGE_DOWN)
		dim += 5;
	// page down
	else if (key == GLFW_KEY_PAGE_UP && dim > 5)
		dim -= 5;
	// change how we initialize particles and reset them all
	else if (key == GLFW_KEY_M) {
		mode = (mode + 1) % maxmodes;
		ResetParticles();
	}
	// pause the simulation
	else if (key == GLFW_KEY_SPACE)
		movement = !movement;
	// move the camera origin / look at point around
	else if (key == GLFW_KEY_W)
		camy += 1;
	else if (key == GLFW_KEY_S)
		camy -= 1;
	else if (key == GLFW_KEY_A)
		camx -= 1;
	else if (key == GLFW_KEY_D)
		camx += 1;
	else if (key == GLFW_KEY_Q)
		camz += 1;
	else if (key == GLFW_KEY_E)
		camz -= 1;

	// keep angles +/-360
	th %= 360;
	ph %= 360;
	// update projection
	Projection(55, asp, dim);
}

// window resize callback
void reshape(GLFWwindow* window, int width, int height) {
	// get framebuffer dimensions
	glfwGetFramebufferSize(window, &width, &height);
	// aspect ratio
	asp = (height > 0) ? (double) width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, width, height);
	// set projection
	Projection(55, asp, dim);
}

int CreateShaderProgCompute(char* file) {
	//  Create program
	int prog = glCreateProgram();
	//  Create and compile compute shader
	CreateShader(prog, GL_COMPUTE_SHADER, file);
	//  Link program
	glLinkProgram(prog);
	//  Check for errors
	PrintProgramLog(prog);
	//  Return name
	return prog;
}

// main program, the beginning of it all
int main(int argc, char* argv[]) {
	// initialize GLFW
	GLFWwindow* window = InitWindow("Kelley Kelley Final Project", 0, 600, 600, &reshape, &key);

	// compute shader
	computeshader = CreateShaderProgCompute("shaders/nbodyandcollide.cs");
	// shader
	colorshader = CreateShaderProg("shaders/nbodyandcollide.vert", "shaders/nbodyandcollide.frag");
	// init particles
	InitParticles();

	// star texture
	//LoadTexBMP("star.bmp");

	// event loop
	ErrCheck("init");
	while (!glfwWindowShouldClose(window)) {
		// display
		display(window);
		// process any events
		glfwPollEvents();
	}
	// shut down GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}