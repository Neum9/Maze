#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
in  vec3 vFaceIndecies;


out vec4 color;
out vec2 texCoord;
out vec4 normal;

uniform vec3 theta;
uniform vec3 translation;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

//光照
out vec3 N;
out vec3 V;

void main()
{
    const float  DegreesToRadians = 3.14159265 / 180.0;

    vec3 c = cos( DegreesToRadians * theta );
    vec3 s = sin( DegreesToRadians * theta );

    mat4 rx = mat4( 1.0, 0.0,  0.0, 0.0,
		    0.0, c.x, -s.x, 0.0,
		    0.0, s.x,  c.x, 0.0,
		    0.0, 0.0,  0.0, 1.0);

    mat4 ry = mat4(   c.y, 0.0, s.y, 0.0,
		      0.0, 1.0, 0.0, 0.0,
		     -s.y, 0.0, c.y, 0.0,
		      0.0, 0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    ry[1][0] = 0.0;
    ry[1][1] = 1.0;

    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
		    s.z,  c.z, 0.0, 0.0,
		    0.0,  0.0, 1.0, 0.0,
		    0.0,  0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    rz[2][2] = 1.0;

    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;
    normal = vec4(vNormal, 0);

 		gl_Position = vec4(vPosition, 1.0);
    gl_Position = rz * ry * rx * gl_Position;
    vec4 t = rz * ry * rx * vec4(translation, 1.0);

    //gl_Position = projMatrix * viewMatrix * modelMatrix * gl_Position + vec4(translation, 0.0);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
     

    //光照
    // TODO 将顶点变换到相机坐标系下
    vec4 vertPos_cameraspace = viewMatrix * modelMatrix * vec4(vPosition,1.0);
    
    // TODO 将法向量变换到相机坐标系下并传入片元着色器
    N =  (viewMatrix * modelMatrix * vec4(vNormal, 0.0)).xyz;
    
    // 对顶点坐标做透视投影
    V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}
