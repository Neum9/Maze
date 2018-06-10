#version 330 core

in vec3 color;
out vec4 fColor;

in vec3 N;
in vec3 V;

uniform mat4 viewMatrix;

uniform vec3 lightPos;
uniform bool isShadow;

void main()
{
	vec4 temp = viewMatrix * vec4(lightPos, 1.0);
	vec3 temp2 = vec3(temp.x,temp.y,temp.z);
	// TODO 设置三维物体的材质属性
	if (isShadow)
	{
		fColor = vec4(0.0,0.0,0.0,1.0);
		return;
	}
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
	
	// TODO 计算最终每个片元的输出颜色

    fColor = 0.5*vec4(color, 1.0) + 0.5*vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, 10.0), 1.0);
}