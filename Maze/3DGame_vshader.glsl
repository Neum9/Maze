#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec3 vColor;
out vec3 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;
uniform bool isShadow;

out vec3 N;
out vec3 V;

uniform vec3 lightPos;

void main()
{
    if (isShadow)
    {
        gl_Position = shadowMatrix * modelMatrix * vec4(vPosition,1.0);
        gl_Position = vec4(gl_Position.xyz/gl_Position.w,1.0);
        gl_Position = projMatrix * viewMatrix * gl_Position;
        return;
    }
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vPosition,1.0);

    color = vColor;
    // TODO 将顶点变换到相机坐标系下
	vec4 vertPos_cameraspace = viewMatrix * modelMatrix * vec4(vPosition,1.0);
	
	// TODO 将法向量变换到相机坐标系下并传入片元着色器
	N =  (viewMatrix * modelMatrix * vec4(vNormal, 0.0)).xyz;
	
	// 对顶点坐标做透视投影
	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}
