#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D uScene;
uniform vec2 iResolution;
uniform float iTime;

void main()
{
    float offset = 0.001;
    vec2 fragCoord = fragTexCoord * iResolution;
    vec2 uv = fragCoord / iResolution;
    float r = texture(uScene,uv+vec2(0.01,0.0)).r;
    float g = texture(uScene,uv).g;
    float b = texture(uScene,uv-vec2(0.01,0.0)).b;

    finalColor = vec4(r, g, b, 1.0);
}

