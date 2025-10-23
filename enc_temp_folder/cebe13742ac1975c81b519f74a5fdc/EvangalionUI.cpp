#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#include <iostream>
#include <print>
#include "raylib-cpp.hpp"
#include <opencv2/opencv.hpp>

int main() {
	cv::Mat frame1;
	cv::Mat frame2;
    cv::Mat framergb;

    Image image_raylib;

    cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		printf("ERROR Cannot open video\n");
	}

	cap.read(frame1);
	cap.read(frame2);

    int screenWidth = frame1.cols;
	int screenHeight = frame1.rows;

    raylib::Window window(1280, 720, "Evangalion UI");
    // Load shader from files (use glsl330 folder for desktop)
    //ToggleFullscreen();

    //--------------------------------------------------------------------------------------
	// Convert from OpenCV to Raylib
	//--------------------------------------------------------------------------------------

    cv::cvtColor(frame2, framergb, cv::COLOR_BGR2RGB);

    image_raylib.width = framergb.cols;
	image_raylib.height = framergb.rows;
	image_raylib.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
	image_raylib.mipmaps= 1 ;
	image_raylib.data= (void*)(framergb.data);
	
	raylib::Texture2D texture(image_raylib);      // Image converted to texture, uploaded to GPU memory (VRAM)


    // Create a render texture to draw the scene first
    raylib::RenderTexture2D target(GetScreenWidth(), GetScreenHeight());

    raylib::Shader shader(NULL,
                               "crt.fs");
    std::cout <<"shader info:" << shader.locs << std::endl;
    //SetTargetFPS(60);
      // Get uniform location(s)
	int resLoc = GetShaderLocation(shader, "iResolution");
	int timeLoc = GetShaderLocation(shader, "iTime");
	Vector2 resolution = { (float)GetScreenWidth(), (float)GetScreenHeight() };

	float time = 0.0f;

	std::atomic<bool> running = true;
	std::mutex frame_mutex;

	std::thread cam_thread([&] {
		cv::VideoCapture cap(0);
        //cap.set(cv::CAP_PROP_FPS, 30);
		while (running) {
			cv::Mat new_frame;
			if (cap.read(new_frame)) {
				std::lock_guard<std::mutex> lock(frame_mutex);
				frame2 = new_frame.clone();
                 cv::cvtColor(new_frame, frame2, cv::COLOR_BGR2RGB);
			}
		}
        cap.release();
		});

    while (!window.ShouldClose())
    {
         cv::Mat localFrame;
        {
            std::lock_guard<std::mutex> lock(frame_mutex);
            if (!frame2.empty()) localFrame = frame2.clone();
        }
        // Paint last caputre frame
		cv::cvtColor(localFrame, framergb, cv::COLOR_BGR2RGB);
		image_raylib.data = (void*)(framergb.data);
        std::lock_guard<std::mutex> lock(frame_mutex);
		texture.Update(image_raylib.data);
        time += GetFrameTime();
        Vector2 resolution = {
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        };
        // --- Draw your game to the offscreen texture ---
        BeginTextureMode(target);
            ClearBackground(BLACK);

            texture.Draw(raylib::Vector2(0,0),WHITE);
            DrawFPS(10, 10);
        EndTextureMode();

        // update uniforms
        SetShaderValue(shader, resLoc, &resolution, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

        BeginDrawing();

            window.ClearBackground(BLACK);

		    BeginShaderMode(shader);
            // Draw the texture covering the entire screen
                   DrawTexturePro(
                       target.texture,
                       raylib::Rectangle( 0, 0, (float)target.texture.width, (float)-target.texture.height ),
                       raylib::Rectangle( 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() ),
                       raylib::Vector2( 0, 0 ),
                       0.0f,
                       WHITE
                   );
            EndShaderMode();


        EndDrawing();
	
    }
    running = false;
    cam_thread.join();
    return 0;
}
