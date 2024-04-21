#version 410 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 SurfaceNormal[];

const float MAGNITUDE = 0.4;
  
layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position + 
                                vec4(SurfaceNormal[index], 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  