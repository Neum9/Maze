#include "Mesh_Painter.h"
#include "FreeImage.h"

Mesh_Painter::Mesh_Painter()
{
}


Mesh_Painter::~Mesh_Painter()
{
}


void Mesh_Painter::draw_meshes(mat4 mm,mat4 vm,int order)
{
	int i = order;
	glUseProgram(this->program_all[i]);//ָ��ʹ����Ⱦ������ͬ��ģ�Ϳ���ָ����ͬ����Ⱦ������������ʹ�õ���Ⱦ��������ͬ���ļ���ѧ�����Ը����Լ��İ��öԲ�ͬ��ģ���趨��ͬ����Ⱦ��
	glBindVertexArray(this->vao_all[i]);
	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);//�������룬����ֻʹ��ͬһ��������л���
		
	float x, y, z;
	this->m_my_meshes_[i]->get_theta(x, y, z);
	GLfloat  Theta[3] = {x, y, z};
	this->m_my_meshes_[i]->add_theta_step();
 	glUniform3fv(theta_all[i], 1, Theta);

	this->m_my_meshes_[i]->get_translate(x, y, z);
	GLfloat  vTranslation[3] = { x, y, z };
	glUniform3fv(trans_all[i], 1, vTranslation);

	//���ݾ���
	glUniformMatrix4fv(ModelMatrixID,1,true,mm);
	glUniformMatrix4fv(ViewMatrixID, 1, true, vm);
	glUniformMatrix4fv(ProjMatrixID, 1, true, this->m_my_meshes_.at(i)->getProjMatrix());
	//�ж��Ƿ񻭻����ˣ��������շ����Ĳ�ͬ
	glUniform1d(IsRobotID, isRobot);
	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
	glUseProgram(0);
};


void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint textures;

		glGenTextures(1, &textures);
		//����FreeImage��������
		load_texture_FreeImage(this->m_my_meshes_[i]->get_texture_file(), textures);

		//ָ�������С���˷����������д��������ӣ������޷���ʾ����
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//�����ɵ�������shader
		glBindTexture(GL_TEXTURE_2D, textures);
		glUniform1i(glGetUniformLocation(this->program_all[i], "texture"), 0);
		this->textures_all.push_back(textures);
	}


};


void Mesh_Painter::load_texture_FreeImage(std::string file_name, GLuint& m_texName)
{
	//1 ��ȡͼƬ��ʽ
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name.c_str(), 0);

	//2 ����ͼƬ
	FIBITMAP *dib = FreeImage_Load(fifmt, file_name.c_str(), 0);

	//3 ת��Ϊrgb 24ɫ;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 ��ȡ����ָ��
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	/**
	* ����һ������Id,������Ϊ��������������Ĳ����������������id
	*/

	/**
	* ʹ���������id,���߽а�(����)
	*/
	glBindTexture(GL_TEXTURE_2D, m_texName);
	/**
	* ָ������ķŴ�,��С�˲���ʹ�����Է�ʽ������ͼƬ�Ŵ��ʱ���ֵ��ʽ
	*/
	/**
	* ��ͼƬ��rgb�����ϴ���opengl.
	*/
	glTexImage2D(
		GL_TEXTURE_2D, //! ָ���Ƕ�άͼƬ
		0, //! ָ��Ϊ��һ�������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
		GL_RGB, //! �����ʹ�õĴ洢��ʽ
		width, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		height, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		0, //! �Ƿ�ı�
		GL_BGR_EXT, //! ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
		GL_UNSIGNED_BYTE, //! ������8bit����
		pixels
		);
	/**
	* �ͷ��ڴ�
	*/
	FreeImage_Unload(dib);
};

void Mesh_Painter::update_vertex_buffer()
{


	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
	
	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		int num_face = this->m_my_meshes_[m_i]->num_faces();
		int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

		const VertexList& vertex_list = this->m_my_meshes_[m_i]->get_vertices();
		const NormalList& normal_list = this->m_my_meshes_[m_i]->get_normals();
		const FaceList&  face_list = this->m_my_meshes_[m_i]->get_faces();
		const STLVectorf& color_list = this->m_my_meshes_[m_i]->get_colors();
		const VtList& vt_list = this->m_my_meshes_[m_i]->get_vts();


		// Create a vertex array object
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec2)*num_face * 3
			,
			NULL, GL_STATIC_DRAW)
			;
		//����㹻�Ŀռ�洢���꣬��ɫ�������Լ���������ȵ�
		// Specify an offset to keep track of where we're placing data in our
		//   vertex array buffer.  We'll use the same technique when we
		//   associate the offsets with vertex attribute pointers.

		vec3* points = new vec3[num_face * 3];
		point3f  p_center_ = this->m_my_meshes_[m_i]->get_center();
		point3f p_min_box_, p_max_box_;
		this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
		float d = p_min_box_.distance(p_max_box_);
		for (int i = 0; i < num_face; i++)
		{
			int index = face_list[3 * i];
			points[3 * i] = vec3(
				vertex_list[index * 3 + 0],
				vertex_list[index * 3 + 1],
				vertex_list[index * 3 + 2]
			);

			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3(
				vertex_list[index * 3 + 0],
				vertex_list[index * 3 + 1],
				vertex_list[index * 3 + 2]
			);

			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3(
				vertex_list[index * 3 + 0],
				vertex_list[index * 3 + 1],
				vertex_list[index * 3 + 2]
			);
		}
		GLintptr offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3)*num_face * 3, points);
		//�������괫��shader
		offset += sizeof(vec3)*num_face * 3;
		delete[] points;
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			int index = face_list[3 * i];
			points[3 * i] = vec3(
				(normal_list[index * 3 + 0]),
				(normal_list[index * 3 + 1]),
				(normal_list[index * 3 + 2])
				);

			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3(
				(normal_list[index * 3 + 0]),
				(normal_list[index * 3 + 1]),
				(normal_list[index * 3 + 2])
				);

			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3(
				(normal_list[index * 3 + 0]),
				(normal_list[index * 3 + 1]),
				(normal_list[index * 3 + 2])
				);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		//���ߴ���shader

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			int index = face_list[3 * i];
			points[3 * i] = vec3(
				(color_list[index * 3 + 0]),
				(color_list[index * 3 + 1]),
				(color_list[index * 3 + 2])
				);

			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3(
				(color_list[index * 3 + 0]),
				(color_list[index * 3 + 1]),
				(color_list[index * 3 + 2])
				);

			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3(
				(color_list[index * 3 + 0]),
				(color_list[index * 3 + 1]),
				(color_list[index * 3 + 2])
				);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		//��ɫ����shader
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		vec2* points_2 = new vec2[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			points_2[i * 3] = vec2(vt_list[i * 6 + 0], vt_list[i * 6 + 1]);
			points_2[i * 3 + 1] = vec2(vt_list[i * 6 + 2], vt_list[i * 6 + 3]);
			points_2[i * 3 + 2] = vec2(vt_list[i * 6 + 4], vt_list[i * 6 + 5]);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
		offset += sizeof(vec2) * num_face * 3;
		delete points_2;
		//�������괫��shader
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		// Load shaders and use the resulting shader program


		// set up vertex arrays
		offset = 0;
		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		//ָ��vPosition��shader��ʹ��ʱ��λ��

		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		//ָ��vNormal��shader��ʹ��ʱ��λ��
		
		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		//ָ��vColor��shader��ʹ��ʱ��λ��
		
		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		//ָ��vTexCoord��shader��ʹ��ʱ��λ��

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);
		
	}
};
void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	this->program_all.clear();
	this->theta_all.clear();
	this->trans_all.clear();
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		//ָ��ʹ�õ���Ⱦ������ͬ��ģ�Ϳ���ָ����ͬ����Ⱦ������������ʹ�õ���Ⱦ��������ͬ���ļ���ѧ�����Ը����Լ��İ��öԲ�ͬ��ģ���趨��ͬ����Ⱦ��
		this->program_all.push_back(program);

		GLuint 	theta = glGetUniformLocation(program, "theta");
		GLuint  trans = glGetUniformLocation(program, "translation");
		theta_all.push_back(theta);
		trans_all.push_back(trans);
		//Add
		ModelMatrixID = glGetUniformLocation(program, "modelMatrix");
		ViewMatrixID = glGetUniformLocation(program, "viewMatrix");
		ProjMatrixID = glGetUniformLocation(program, "projMatrix");
		LightID = glGetUniformLocation(program, "lightPos");
		IsRobotID = glGetUniformLocation(program, "isRobot");
	}

};
void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};
void Mesh_Painter::clear_mehs()
{
	this->m_my_meshes_.clear();

	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
}
void Mesh_Painter::setLightPos(vec3 p)
{
	for (int i = 0;i < this->m_my_meshes_.size();i++)
	{
		glUseProgram(this->program_all[i]);
		glUniform3fv(LightID, 1, &p[0]);
	}
}
void Mesh_Painter::setIsRobot(bool b)
{
	isRobot = b;
}
;
