/*
* Kelley Kelley
* Final Project
* CSCI 4239/5239 Spring 2022
* 
* Soooo...I could've done what I was originally planning with the whole forest thing, 
bbuuuttttt, that was really lame to me and felt like I was just putting shaders on my last project 
and it'd be mostly the same just like, better. And that was lame. So instead, I decided to make a star formation simulator. 
* 
* Key bindings:
esc - quit
0 - reset view
r - reset particles
arrows - change view
pgup/pgdn - zoom in and out
*/
#include "mylib.h"
double dim = 10; // size of the world
int th = 0; // azimuth of view angle
int ph = 0; // elevation of view angle
int nw, ng; // work group size and count
int n; // number of particles
double asp = 1; // aspect ratio
int computeshader; // compute shader program
int colorshader; // shader program
unsigned int posbuf; // position buffers
unsigned int velbuf; // double buffer
unsigned int colbuf; // color buffer

typedef struct {
	union { float x; float r; };
	union { float y; float g; };
	union { float z; float b; };
	union { float w; float a; };
} vec4;

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
	vec4 *pos, *vel, *col;
	// reset position
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf);
	pos = (vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++) {
		pos[i].x = frand(-50, 50);
		pos[i].y = frand(-50, 50);
		pos[i].z = frand(-50, 50);
		pos[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset velocities
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf);
	vel = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		vel[i].x = frand(-10, +10);
		vel[i].y = frand(-10, +10);
		vel[i].z = frand(-10, +10);
		vel[i].w = 0;
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
		col[i].a = 1.;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Set buffer base
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posbuf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, velbuf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, colbuf);

	//  Unset buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// init particles
void InitParticles() {
	// get max workgroup size and count
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &ng);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &nw);
	if (ng > 16) ng = 16;
	if (nw > 512) nw = 512;
	n = nw * ng;

	// initialize position buffers
	glGenBuffers(1, &posbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(vec4), NULL, GL_STATIC_DRAW);

	// initialize velocity buffer
	glGenBuffers(1, &velbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf);
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
	
	glUseProgram(colorshader);
	glGenVertexArrays(1, &icoVAO);
	glBindVertexArray(icoVAO);
	glBindBuffer(GL_ARRAY_BUFFER, icoVBO);
	int loc = glGetAttribLocation(colorshader, "Vertex");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);

	glBindBuffer(GL_ARRAY_BUFFER, posbuf);
	loc = glGetAttribLocation(colorshader, "translation");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);
	glVertexAttribDivisor(loc, 1);

	glBindBuffer(GL_ARRAY_BUFFER, colbuf);
	loc = glGetAttribLocation(colorshader, "color");
	glVertexAttribPointer(loc, 4, GL_FLOAT, 0, 0, (void*)0);
	glEnableVertexAttribArray(loc);
	glVertexAttribDivisor(loc, 1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	// reset buffer positions
	ResetParticles();
}

// draw the particles
void DrawParticles() {
	// set shader
	glUseProgram(colorshader);

	// projection matrix
	float proj[16];
	mat4identity(proj);
	mat4ortho(proj, -dim * asp, +dim * asp, -dim, +dim, -dim, +dim);
	// view matrix
	float view[16];
	mat4identity(view);
	mat4rotate(view, ph, 1, 0, 0);
	mat4rotate(view, th, 0, 1, 0);
	// modelview matrix
	float modelview[16];
	mat4copy(modelview, view);
	mat4translate(modelview, 0, 0, 0.5);

	int id = glGetUniformLocation(colorshader, "ProjectionMatrix");
	glUniformMatrix4fv(id, 1, 0, proj);
	id = glGetUniformLocation(colorshader, "ModelViewMatrix");
	glUniformMatrix4fv(id, 1, 0, modelview);

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
	// reset view angle
	else if (key == GLFW_KEY_0)
		th = ph = 0;
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