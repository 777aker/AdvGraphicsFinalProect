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
unsigned int posbuf1; // position buffers
unsigned int posbuf2; // double buffer
unsigned int velbuf1; // velocity buffers
unsigned int velbuf2; // double buffer
unsigned int colbuf; // color buffer
int buf; // buffer value

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
		pos1[i].x = frand(-10, 10);
		pos1[i].y = frand(-10, 10);
		pos1[i].z = frand(-10, 10);
		pos1[i].w = 1;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posbuf2);
	pos2 = (vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
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
	if (ng > 16) ng = 16;
	if (nw > 512) nw = 512;
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
	// set shader
	glUseProgram(colorshader);
	// set up texture and size
	int id = glGetUniformLocation(colorshader, "star");
	if(id>=0) glUniform1i(id, 0);
	id = glGetUniformLocation(colorshader, "size");
	if(id>=0) glUniform1f(id, .1);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	// set particle size
	glPointSize(1);
	// vertex array
	if (buf)
		glBindBuffer(GL_ARRAY_BUFFER, posbuf1);
	else
		glBindBuffer(GL_ARRAY_BUFFER, posbuf2);
	glVertexPointer(4, GL_FLOAT, 0, (void*)0);
	// color array
	glBindBuffer(GL_ARRAY_BUFFER, colbuf);
	glColorPointer(4, GL_FLOAT, 0, (void*)0);
	// enable arrays used by drawarrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// draw arrays
	glDrawArrays(GL_POINTS, 0, n);
	// disable arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	// reset buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// turn off blend
	glDisable(GL_BLEND);
	// return to default shader
	glUseProgram(0);
}

// run compute shader
void compute() {
	glUseProgram(computeshader);
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
	buf = (buf + 1) % 2;
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
	glClear(GL_COLOR_BUFFER_BIT);
	// set eye position
	View(th, ph, 55, dim);

	// compute shader
	compute();

	// draw the particles
	DrawParticles();

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
	GLFWwindow* window = InitWindow("Kelley Kelley Final Project", 1, 600, 600, &reshape, &key);
	
	// compute shader
	computeshader = CreateShaderProgCompute("shaders/nbodyandcollide.cs");
	// texturing shader
	colorshader = CreateShaderProgGeom("shaders/nbodyandcollide.vert", "shaders/nbodyandcollide.geom", "shaders/nbodyandcollide.frag");
	// init particles
	InitParticles();

	// star texture
	LoadTexBMP("star.bmp");

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