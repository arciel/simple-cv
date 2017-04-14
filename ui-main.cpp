
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "GL/gl3w.h"
#include <SDL.h>


#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

#include "cv-pipelines.h"
#include "settings.h"

#define APP_WINTITLE "OpenCV demo/sample for enCypher" //TODO : Hash this.

#define SAMPLE(X) "C:\\dhoomketu\\samples\\" ##X

#define arg 0

GLuint matToTexture(const cv::Mat &mat, GLenum wrapFilter);
void update_tex_mat(GLuint tex, const cv::Mat &m);

//feed boxen
void ui_srcfeed(const cv::Mat& frame, GLuint);
void ui_gsfeed(const cv::Mat& frame, GLuint);
void ui_invfeed(const cv::Mat& frame, GLuint);
void ui_blurfeed(const cv::Mat& frame, GLuint);
void ui_cannyfeed(const cv::Mat& frame, GLuint);
void ui_isofeed(const cv::Mat& frame, GLuint);
void ui_mog2feed(const cv::Mat& frame, GLuint);

enum { FSRC = 0, FGS, FINV, FBLUR, FCNY, FISO, FCMOG, FMOGCT, FNUM };
static bool feed_enabled[FNUM] = { true };
using feedptr = void(*) (const cv::Mat&, GLuint);
feedptr boxen[FNUM] = { ui_srcfeed, ui_gsfeed, ui_invfeed, ui_blurfeed, ui_cannyfeed, ui_isofeed, ui_mog2feed, ui_mog2feed };
static GLuint texen[FNUM] = {};

static cv::VideoCapture cam{ arg };


int main(int, char**)
{
	// Setup SDL

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	// Setup window and OpenGL flags
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	SDL_Window* window = SDL_CreateWindow(APP_WINTITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	gl3wInit();

	// Setup ImGui binding
	ImGui_ImplSdlGL3_Init(window);


	

	
	cv::Mat frame;
	
	settings::get().cap_width  = cam.get(cv::CAP_PROP_FRAME_WIDTH);
	settings::get().cap_height = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
	settings::get().cap_ar = settings::get().cap_width / settings::get().cap_height; // calculate aspect ratio
	settings::get().cap_scale = 0.50;

	// scale down the camera feed to occupy half the area.
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, settings::get().cap_height * settings::get().cap_scale);
	cam.set(cv::CAP_PROP_FRAME_WIDTH, settings::get().cap_width * settings::get().cap_scale * settings::get().cap_ar);

	cam >> frame;
	//texen[FSRC] = matToTexture(frame, GL_CLAMP_TO_EDGE);
	for (int i = FSRC; i != FNUM; i++)
		texen[i] = matToTexture(frame, GL_CLAMP_TO_EDGE);

	// Main loop
	bool running = true;
	while (running)
	{
		cam >> frame;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				running = false;
		}
		ImGui_ImplSdlGL3_NewFrame(window);
		
		//Draw UI Boxes
		for (int i = FSRC; i != FNUM; i++)
		{
			if (feed_enabled[i]) boxen[i](frame, texen[i]);
		}
		// Rendering
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		glClearColor(0.5, 0.5, 0.5, 0.75);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		SDL_GL_SwapWindow(window);

		//cam >> img;
		//fin = pipeline_canny(img);

		//update_tex_mat(img_tx, img);
		//update_tex_mat(fin_tx, fin);
	}

	// Cleanup
	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}


// Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
GLuint matToTexture(const cv::Mat &mat, GLenum wrapFilter = GL_CLAMP_TO_EDGE)
{
	// Generate a number for our textureID's unique handle
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Bind to our texture handle
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLenum	magFilter = GL_LINEAR;
	GLenum minFilter = GL_NEAREST;
	
	// Set texture interpolation methods for minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

	GLenum inputColourFormat = mat.channels() == 1 ? GL_RED : GL_BGR;
	
	// Create the texture
	glTexImage2D(GL_TEXTURE_2D,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
		mat.rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		mat.ptr());        // The actual image data itself

	return textureID;
}

void update_tex_mat(GLuint tex, const cv::Mat &m)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m.cols, m.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, m.ptr());
}

void ui_srcfeed(const cv::Mat& frame, GLuint frtex)
{
	update_tex_mat(frtex, frame);
	ImGui::Begin("Source Feed", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(frame.rows, frame.cols)); //TODO : Match aspect ratio
	//Show a "right-click" context menu on this frame to enable the user to select effects
	bool b = true;
	if (ImGui::BeginPopupContextItem("Effects & Filters"))
	{
		ImGui::Selectable("Quit", &b);
		ImGui::Selectable("Grayscale", &feed_enabled[FGS]);
		ImGui::Selectable("Canny Edge Detection", &feed_enabled[FCNY]);
		ImGui::Selectable("Color Inversion", &feed_enabled[FINV]);
		ImGui::Selectable("Color Isolation", &feed_enabled[FISO]);
		ImGui::Selectable("Gaussian Blur", &feed_enabled[FBLUR]);
		ImGui::Selectable("Motion Detection w/ Color Isolation", &feed_enabled[FCMOG]);
		ImGui::Selectable("Motion Detection w/ Contour Plots", &b);
		ImGui::Selectable("Motion Detection w/ Box Models", &b);
		ImGui::EndPopup();
	}
	ImGui::Separator();
	ImGui::TextWrapped("Right-click on the source feed to access effects and filters");
	ImGui::Separator();
	//ImGui::TextWrapped("Feed Options");
	ImGui::SliderFloat("", &settings::get().cap_scale, 0.1f, 1.0f);
	if (ImGui::Button("Go!"))
	{
		cam.set(cv::CAP_PROP_FRAME_HEIGHT, settings::get().cap_height * settings::get().cap_scale);
		cam.set(cv::CAP_PROP_FRAME_WIDTH, settings::get().cap_width * settings::get().cap_scale * settings::get().cap_ar);
	}
	//Todo : Sliders
	ImGui::End();
}

void ui_gsfeed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_grayscale(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Greyscale Feed", &feed_enabled[FGS], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Greyscale Effect");
	ImGui::End();
}

void ui_invfeed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_invert(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Color Inversion", &feed_enabled[FINV], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Color Inversion");
	ImGui::Separator();
	ImGui::End();
}

void ui_blurfeed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_blur(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Gaussian Blur", &feed_enabled[FBLUR], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Gaussian Blur");
	ImGui::Separator();
	ImGui::TextWrapped("Strength along X:");
	ImGui::TextWrapped("Strength along Y:");
	ImGui::End();
}

void ui_cannyfeed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_canny(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Canny Edge Detection", &feed_enabled[FCNY], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Canny Edge Effect");
	ImGui::Separator();
	ImGui::Checkbox("Smoothen?", &settings::get().cannyblur);
	//ImGui::Checkbox
	ImGui::TextWrapped("Lower Threshold");
	ImGui::TextWrapped("Upper Threshold");
	ImGui::TextWrapped("Aperture Size");
	

	ImGui::End();
}

void ui_isofeed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_1color(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Color Isolation", &feed_enabled[FISO], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Color Isolation");
	ImGui::End();
}

void ui_mog2feed(const cv::Mat& frame, GLuint frtex)
{
	cv::Mat r = pipeline_colorMOG(frame);
	update_tex_mat(frtex, r);
	ImGui::Begin("Motion Detection w/ Color Isolation", &feed_enabled[FCMOG], ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::ImageButton((void*)frtex, ImVec2(r.rows, r.cols));
	ImGui::Separator();
	ImGui::TextWrapped("Motion Detection w/ Color Isolation");
	ImGui::End();
}






