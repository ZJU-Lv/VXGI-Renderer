#version 330 core
#define M_PI 3.1415926535

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 ProjectiomFromXAxis;
uniform mat4 ProjectiomFromYAxis;
uniform mat4 ProjectiomFromZAxis;

in VertexData
{
    vec3 normal;
    vec2 texCoord;
    vec4 positionLightSpace;
} vertexOut[];

out GeomData
{
    vec3 normal;
    vec2 texCoord;
    flat int axis;            // 1 represent X, 2 represent Y, 3 represent Z
    vec4 positionLightSpace;
} geomOut;

void main()
{
    vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(edge1, edge2));

    float normalX = abs(normal.x);
    float normalY = abs(normal.y);
    float normalZ = abs(normal.z);

    mat4 projectionMatrix;

    if(normalX > normalY && normalX > normalZ)
    {
        geomOut.axis = 1;
        projectionMatrix = ProjectiomFromXAxis;
    }
    else if(normalY > normalX && normalY > normalZ)
    {
        geomOut.axis = 2;
        projectionMatrix = ProjectiomFromYAxis;
    }
    else
    {
        geomOut.axis = 3;
        projectionMatrix = ProjectiomFromZAxis;
    }
    
    for(int i = 0; i < gl_in.length(); i++)
    {
        geomOut.normal = vertexOut[i].normal;
        geomOut.texCoord = vertexOut[i].texCoord;
        geomOut.positionLightSpace = vertexOut[i].positionLightSpace;
        gl_Position = projectionMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    
    EndPrimitive();
}