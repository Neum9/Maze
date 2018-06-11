#version 330 core

in vec4 color;
in vec2 texCoord;
in vec4 normal;
in vec3 faceIndecies;

out vec4 fColor;
out vec4 fNormal;

uniform mat4 viewMatrix;

uniform bool isRobot;

//光照
in vec3 N;
in vec3 V;
uniform vec3 lightPos;

uniform sampler2D texture;

void main()
{
	vec4 temp = viewMatrix * vec4(lightPos, 1.0);
	vec3 temp2 = vec3(temp.x,temp.y,temp.z);
    //光照
    // TODO 设置三维物体的材质属性
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);

	// TODO 计算N，L，V，R四个向量并归一化
	vec3 N_norm = normalize(N);
	vec3 L_norm  = normalize(temp2 - V);
	vec3 E_norm  = normalize(-V); 	
	vec3 R_norm  = reflect(-L_norm, N_norm);


	// TODO 计算漫反射系数和镜面反射系数
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular = clamp(dot(R_norm, E_norm), 0.0, 1.0);
	


		//fColor = vec4(1, 0, 0, 0 ) * texture2D( texture, texCoord );
    fColor = texture2D( texture, texCoord );
    //fColor = color;
    //fColor = vec4(0, 0, 0, 0 );
    fNormal = normal;


	// TODO 计算最终每个片元的输出颜色
	if (isRobot)
	{
	    fColor = 0.2* vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, 10.0), 1.0) + 0.8 * fColor;
    }
    else
    {
        fColor = 0.5 * vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, 10.0), 1.0) + 0.5 * fColor;
    }
}

