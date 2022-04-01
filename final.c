/*
* Kelley Kelley
* Final Project
* CSCI 4239/5239 Spring 2022
* 
* Key Bindings:
*/
#include "mylib.h"
double dim = 100; // size of the world
int fov = 55; // field of view
double asp = 1; // aspect ratio

int gensize = 200; // how far out from the center we generate quads

double light_pos = 0; // light position (obviously)
int light_distance = 100; // light distance from center (rotation radius)
bool light_move = true; // for testing

float position[3] = { 0, 0, 0 }; // perlin offset
float view_angle[2] = { 0, 0 }; // view angle for first person camera, want to try up and down so using two angles

// refresh display
//void display(GLFWwindow* window) {

//}

// window resize callback
void reshape(GLFWwindow* window, int width, int height) {
	// get framebuffer dimensions
	glfwGetFramebufferSize(window, &width, &height);
	// aspect ratio
	asp = (height > 0) ? (double) width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, width, height);
}

// main program, the beginning of it all
int main(int argc, char* argv[]) {
	// initialize GLFW
	GLFWwindow* window = InitWindow("Kelley Kelley Final Project", 1, 600, 600, &reshape, &key);
	
	// event loop
	ErrCheck("init");
	while (!glfwWindowShouldClose(window)) {
		// light animation
		light_pos = fmod(90 * glfwGetTime(), 360);
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