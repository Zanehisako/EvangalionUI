#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#include <iostream>
#include <print>
#include "raylib-cpp.hpp"

int main() {
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;

    raylib::Window window(screenWidth, screenHeight, "Evangalion UI");
    // Load shader from files (use glsl330 folder for desktop)
    ToggleFullscreen();

	constexpr int rectWidth = 100;
	constexpr int rectHeight= 100;
    raylib::Rectangle rect(screenWidth/2-rectWidth, screenHeight/2-rectHeight,rectWidth,rectHeight);


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

    while (!window.ShouldClose())
    {
        time += GetFrameTime();
        Vector2 resolution = {
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        };
        // --- Draw your game to the offscreen texture ---
        BeginTextureMode(target);
            ClearBackground(BLACK);

            rect.DrawGradientV(GREEN,RED);
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

    return 0;
}
