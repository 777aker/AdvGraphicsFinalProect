/*
* Kelley Kelley
* Final Project
* CSCI 4239/5239 Spring 2022
* 
* Key Bindings:
*/
#include "mylib.h"
double dim = 1000; // size of the world
int th = 0; // azimuth of view angle
int ph = 0; // elevation of view angle
int nw, ng; // work group size and count
int n; // number of particles
double asp = 1; // aspect ratio
int computeshader; // compute shader program
unsigned int posbuf1; // position buffers
unsigned int posbuf2; // double buffer
unsigned int velbuf1; // velocity buffers
unsigned int velbuf2; // double buffer
unsigned int colbuf; // color buffer

typedef struct {
	union { float x; float r; };
	union { float y; float g; };
	union { float z; float b; };
	union { float w; float a; };
} vec4;

// particles
void ResetParticles() {
	vec4 *pos1, *pos2, *vel1, *vel2, *col;
	// reset position
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf1);
	pos1 = (vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++) {
		pos1[i].x = frand(0, 100);
		pos1[i].y = frand(+400, +600);
		pos1[i].z = frand(-50, +50);
		pos1[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf2);
	pos2 = (vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++) {
		pos2[i].x = 0;
		pos2[i].y = 0;
		pos2[i].z = 0;
		pos2[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset velocities
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf1);
	vel1 = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		vel1[i].x = frand(-10, +10);
		vel1[i].y = frand(-10, +10);
		vel1[i].z = frand(-10, +10);
		vel1[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset velocities
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velbuf2);
	vel2 = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		vel2[i].x = 0;
		vel2[i].y = 0;
		vel2[i].z = 0;
		vel2[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//  Reset colors
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colbuf);
	col = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (int i = 0; i < n; i++)
	{
		col[i].r = frand(0.1, 1.0);
		col[i].g = frand(0.1, 1.0);
		col[i].b = frand(0.1, 1.0);
		col[i].a = 1.;
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
	if (ng > 8192) ng = 8192;
	if (nw > 1024) nw = 1024;
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

	// reset buffer positions
	ResetParticles();
}

// draw the particles
void DrawParticles() {
	// set particle size
	glPointSize(1);
	// vertex array
	glBindBuffer(GL_ARRAY_BUFFER, posbuf1);
	glVertexPointer(4, GL_FLOAT, 0, (void*)0);

}

// refresh display
void display(GLFWwindow* window) {
	// erase window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// set eye position
	View(th, ph, 55, dim);

	// compute shader
	glUseProgram(computeshader);
	glDispatchCompute(ng, 1, 1);
	glUseProgram(0);
	// wait for compute shader
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// draw the particles
	DrawParticles();

	// display parameters
	glDisable(GL_DEPTH_TEST);
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
		dim += 10;
	// page down
	else if (key == GLFW_KEY_PAGE_UP && dim > 300)
		dim -= 10;

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
	GLFWwindow* window = InitWindow("Kelley Kelley Final Project", 1, 600, 600, &reshape, &key);
	
	// compute shader
	computeshader = CreateShaderProgCompute("shaders/nbodyandcollide.cs");
	// init particles
	InitParticles();

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