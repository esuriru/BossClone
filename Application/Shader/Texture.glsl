#type vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColour;
layout(location = 2) in vec2 vertexTexCoords;
layout(location = 3) in float vertexTexIndex;
layout(location = 4) in float tilingFactor;

out vec2 v_texCoords;
out vec4 v_Colour;
out float v_TexIndex;
out float v_tilingFactor;

uniform mat4 u_viewProjection;
uniform mat4 u_model;

void main()
{
    v_Colour = vertexColour;
    v_TexIndex = vertexTexIndex;
    v_texCoords = vertexTexCoords;
    v_tilingFactor = tilingFactor;
    gl_Position = u_viewProjection * vec4(vertexPosition, 1.0);
}

#type pixel
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoords;
in vec4 v_Colour;
in float v_TexIndex;
in float v_tilingFactor;

uniform float u_tilingFactor;
uniform vec4 u_Color;
uniform sampler2D u_Textures[32]; // FOR NOW, MAXIMUM 32 TEXTURES.

void main()
{
    // Allegedly NVIDIA cards can only handle this code.
    // color = texture(u_Textures[int(vTexIndex)], texCoords * v_tilingFactor)  * v_Colour;
    vec4 texColor = v_Colour;

    // Have to do this because AMD cards cannot handle the above code.
    switch(int(v_TexIndex))
    {
        case 0: texColor *= texture(u_Textures[0], v_texCoords * v_tilingFactor); break;
        case 1: texColor *= texture(u_Textures[1], v_texCoords * v_tilingFactor); break;
        case 2: texColor *= texture(u_Textures[2], v_texCoords * v_tilingFactor); break;
        case 3: texColor *= texture(u_Textures[3], v_texCoords * v_tilingFactor); break;
        case 4: texColor *= texture(u_Textures[4], v_texCoords * v_tilingFactor); break;
        case 5: texColor *= texture(u_Textures[5], v_texCoords * v_tilingFactor); break;
        case 6: texColor *= texture(u_Textures[6], v_texCoords * v_tilingFactor); break;
        case 7: texColor *= texture(u_Textures[7], v_texCoords * v_tilingFactor); break;
        case 8: texColor *= texture(u_Textures[8], v_texCoords * v_tilingFactor); break;
        case 9: texColor *= texture(u_Textures[9], v_texCoords * v_tilingFactor); break;
        case 10:texColor *= texture(u_Textures[10], v_texCoords * v_tilingFactor); break;
        case 11:texColor *= texture(u_Textures[11], v_texCoords * v_tilingFactor); break;
        case 12:texColor *= texture(u_Textures[12], v_texCoords * v_tilingFactor); break;
        case 13:texColor *= texture(u_Textures[13], v_texCoords * v_tilingFactor); break;
        case 14:texColor *= texture(u_Textures[14], v_texCoords * v_tilingFactor); break;
        case 15:texColor *= texture(u_Textures[15], v_texCoords * v_tilingFactor); break;
        case 16:texColor *= texture(u_Textures[16], v_texCoords * v_tilingFactor); break;
        case 17:texColor *= texture(u_Textures[17], v_texCoords * v_tilingFactor); break;
        case 18:texColor *= texture(u_Textures[18], v_texCoords * v_tilingFactor); break;
        case 19:texColor *= texture(u_Textures[19], v_texCoords * v_tilingFactor); break;
        case 20:texColor *= texture(u_Textures[20], v_texCoords * v_tilingFactor); break;
        case 21:texColor *= texture(u_Textures[21], v_texCoords * v_tilingFactor); break;
        case 22:texColor *= texture(u_Textures[22], v_texCoords * v_tilingFactor); break;
        case 23:texColor *= texture(u_Textures[23], v_texCoords * v_tilingFactor); break;
        case 24:texColor *= texture(u_Textures[24], v_texCoords * v_tilingFactor); break;
        case 25:texColor *= texture(u_Textures[25], v_texCoords * v_tilingFactor); break;
        case 26:texColor *= texture(u_Textures[26], v_texCoords * v_tilingFactor); break;
        case 27:texColor *= texture(u_Textures[27], v_texCoords * v_tilingFactor); break;
        case 28:texColor *= texture(u_Textures[28], v_texCoords * v_tilingFactor); break;
        case 29:texColor *= texture(u_Textures[29], v_texCoords * v_tilingFactor); break;
        case 30:texColor *= texture(u_Textures[30], v_texCoords * v_tilingFactor); break;
        case 31:texColor *= texture(u_Textures[31], v_texCoords * v_tilingFactor); break;
    }
    color = texColor;
}