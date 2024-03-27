#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat2 transform;
  vec2 offset;
  vec3 color;
} push;

void main(){  
    //直接使用推送进来的数据
     gl_Position = vec4(push.transform*position + push.offset,0.0,1.0);
     //fragColor = push.color;
     fragColor = color;
}


