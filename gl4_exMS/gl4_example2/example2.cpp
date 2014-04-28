/*
 *  OpenGL 4 Sample	1
 *	Author: Ruijin Wu <ruijin@cise.ufl.edu>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"

using namespace std;

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WINDOW_TITLE_PREFIX "OpenGL 4 Sample"



typedef struct
{
	float XYZW[4];
	float RGBA[4];
	float TEX[3];
} Vertex;	

enum CubicType { Serpentine, Cusp, Loop, Quadratic, Line, Point };

class CubicSpline {

public:

	glm::vec3 _vertices[4];
	float _d1;
	float _d2;
	float _d3;
    float _discr;
	CubicType _type;
	float _kFunc[4];
	float _lFunc[4];
	float _mFunc[4];
	glm::mat4 KLM;

	float _ls;
	float _lt;
	float _ms;
	float _mt;

	void CalculateDs();
	void SetType();
	void CalculateLs();
	void CalculateMs();
	void CalculateFunctionals();
	void CreateKLM();

	CubicSpline();
	CubicSpline(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
	CubicSpline(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
};

// default ctor
CubicSpline::CubicSpline()
{
}

CubicSpline::CubicSpline(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
{
	glm::vec3 b0(x0, y0, 1.0);
	glm::vec3 b1(x1, y1, 1.0);
	glm::vec3 b2(x2, y2, 1.0);
	glm::vec3 b3(x3, y3, 1.0);

	_vertices[0] = b0;
	_vertices[1] = b1;
	_vertices[2] = b2;
	_vertices[3] = b3;

	float x = _vertices[0].x;

	CalculateDs();
	SetType();
	CalculateLs();
	CalculateMs();
	CalculateFunctionals();
	CreateKLM();
}

//CubicSpline::CubicSpline(glm::vec3 b0, glm::vec3 b1, glm::vec3 b2, glm::vec3 b3)
//{
//	_vertices[0] = b0;
//	_vertices[1] = b1;
//	_vertices[2] = b2;
//	_vertices[3] = b3;
//
//	CalculateDs();
//	SetType();
//	CalculateLs();
//	CalculateMs();
//	CalculateFunctionals();
//}

void CubicSpline::CalculateDs()
{
	float a1 = glm::dot( _vertices[0], glm::cross(_vertices[3], _vertices[2]));
	float a2 = glm::dot( _vertices[1], glm::cross(_vertices[0], _vertices[3]));
	float a3 = glm::dot( _vertices[2], glm::cross(_vertices[1], _vertices[1]));
	_d1 = a1 - 2*a2 + 3*a3;
	_d2 = -a2 + 3*a3;
	_d3 = 3*a3;

	_discr = _d1*_d1 *(3*_d2*_d2 - 4 * _d1 * _d3);


}

void CubicSpline::SetType()
{
	if (_vertices[0] == _vertices[1] && _vertices[1] == _vertices[2] && _vertices[2] == _vertices[3])
	{
		_type = Point;
	}
	else if (_d1 == _d2 && _d2 == _d3 && _d3 == 0)
	{
		_type = Line;
	}
	else if (_d1 == _d2 && _d2 == _d3 && _d3 != 0)
	{
		_type = Quadratic;
	}
	else if (_discr > 0.0)
	{
		_type = Serpentine;
	}
	else if (_discr == 0.0)
	{
		_type = Cusp;
	}
	else if (_discr < 0.0)
	{
		_type = Loop;
	}
}

void CubicSpline::CalculateLs()
{
	switch (_type)
	{
		case Serpentine:
			_ls = 3.0*_d2 - sqrt((9.0*pow(_d2,2)-12.0*_d1*_d3));
			_lt = 6.0*_d1;
			break;
		case Cusp:
				// TODO: set _ls and _lt
			break;
		case Loop:
			_ls = _d2 - sqrt((4*_d1*_d3)-(3*(pow(_d2,2))));
			_lt = 2*_d1;
			break;
		case Quadratic:
				// TODO: set _ls and _lt
			break;
		default:
			break;
	}
}
void CubicSpline::CalculateMs()
{
	switch (_type)
	{
		case Serpentine:
			_ms = 3.0*_d2 - sqrt((9.0*pow(_d2,2)-12.0*_d1*_d3));
			_mt = 6.0*_d1;
			break;
		case Cusp:
			// TODO: set _ms and _mt
			break;
		case Loop:
			_ms = _d2 - sqrt((4*_d1*_d3)-(3*(pow(_d2,2))));
			_mt = 2*_d1;
			break;
		case Quadratic:
			// TODO: set _ms and _mt
			break;
		default:
			break;
	}
}

void CubicSpline::CalculateFunctionals()
{
	switch (_type)
	{
		case Serpentine:
				_kFunc[0] = _ls*_ms;
				_kFunc[1] = (1.0/3.0)*(3.0*_ls*_ms -_ls*_mt-_lt*_ms);
				_kFunc[2] = (1.0/3.0)*(
					_lt*(_mt-2.0*_ms)+_ls*(3.0*_ms-2.0*_mt)
					);
				_kFunc[3] = (_lt-_ls)*(_mt-_ms);

				_lFunc[0] = pow(_ls, 3);
				_lFunc[1] = pow(_ls, 2)*(_ls-_lt);
				_lFunc[2] = pow((_lt-_ls), 2)*(_ls);
				_lFunc[3] = -pow((_lt-_ls), 3);

				_mFunc[0] = pow(_ms, 3);
				_mFunc[1] = pow(_ms, 2)*(_ms-_mt);
				_mFunc[2] = pow((_mt-_ms), 2)*(_ms);
				_mFunc[3] = -pow((_mt-_ms), 3);
			break;
		case Cusp:
				_kFunc[0] = _ls;
				_kFunc[1] = _ls - (1/3)*_lt;
				_kFunc[2] = _ls - (2/3)*_lt;
				_kFunc[3] = (_ls-_lt);

				_lFunc[0] = pow(_ls, 3);
				_lFunc[1] = pow(_ls,2)*(_ls-_lt);
				_lFunc[2] = pow((_ls-_lt),2)*_ls;
				_lFunc[3] = pow((_ls-_lt), 3);

				_mFunc[0] = 1;
				_mFunc[1] = 1;
				_mFunc[2] = 1;
				_mFunc[3] = 1;
			break;
		case Loop:
				_kFunc[0] = _ls*_ms;
				_kFunc[1] = (1.0/3.0)*(-1*_ls*_mt -_lt*_ms-_ls*_ms);
				_kFunc[2] = (1.0/3.0)*(
					_lt*(_mt-2.0*_ms)+_ls*(3.0*_ms-2.0*_mt)
					);
				_kFunc[3] = (_lt-_ls)*(_mt-_ms);

				_lFunc[0] = pow(_ls, 2)*_ms;
				_lFunc[1] = (-1/3)*_ls*(_ls*(_mt - 3*_ms) + 2*_lt*_ms);
				_lFunc[2] = (1/3)*(_lt - _ls)*(_ls*(2*_mt - 3*_ms) + _lt*_ms);
				_lFunc[3] = -pow((_lt-_ls), 2)*(_mt-_ms);

				_mFunc[0] = _ls*pow(_ms, 2);
				_mFunc[1] = (-1/3)*_ms*(_ls*(2*_mt - 3*_ms) + _lt*_ms);
				_mFunc[2] = (1/3)*(_mt-_ms)*(_ls*(_mt - 3*_ms) + 2*_lt*_ms);
				_mFunc[3] = -1*(_lt-_ls)*pow((_mt-_ms), 2);

			break;
		case Quadratic:
				// TODO: set _kFunc[0-4], _lFunc[0-4] and _mFunc[0-4]
			break;
		default:
			break;
	}
}
void CubicSpline::CreateKLM(){
	KLM = glm::mat4(1.0);
	KLM[0].x = _kFunc[0];
	KLM[0].y = _lFunc[0];
	KLM[0].z = _mFunc[0];
	
	KLM[1].x = _kFunc[1];
	KLM[1].y = _lFunc[1];
	KLM[1].z = _mFunc[1];

	
	KLM[2].x = _kFunc[2];
	KLM[2].y = _lFunc[2];
	KLM[2].z = _mFunc[2];
	
	KLM[3].x = _kFunc[3];
	KLM[3].y = _lFunc[3];
	KLM[3].z = _mFunc[3];


	//KLM[0] = glm::vec4(_kFunc[0], _kFunc[1], _kFunc[2], _kFunc[3]);
	//KLM[1] = glm::vec4(_lFunc[0], _lFunc[1], _lFunc[2], _lFunc[3]);
	//KLM[2] = glm::vec4(_mFunc[0], _mFunc[1], _mFunc[2], _mFunc[3]);
}


int CurrentWidth = 600,
	CurrentHeight = 600,
	WindowHandle = 0;

unsigned FrameCount = 0;

float TessLevelInner = 1.0f; 
float TessLevelOuter = 1.0f;

glm::mat4 ModelMatrix;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 globalKlm;
GLuint klmLocation;

vector<CubicSpline> splines;

// animation control
bool animation = true;
bool displacement = false;

GLuint
	VertexShaderId,
	FragmentShaderId,
	TessControlShaderId,
	TessEvalShaderId,
	ProgramId,
	VaoId,
	BufferId,
	IndexBufferId;

GLuint TessLevelInnerLocation,
	TessLevelOuterLocation;

GLuint ModelMatrixLocation,
	ViewMatrixLocation,
	ProjectionMatrixLocation;

GLuint DisplacementLocation;

void Initialize(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void KeyboardFunction(unsigned char, int, int);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);

void loadSvg(){
	struct NSVGimage* image;
	image = nsvgParseFromFile("CLetter.svg", "px", 96);
	printf("size: %f x %f\n", image->width, image->height);
	// Use...
	NSVGshape* shape;
	NSVGpath* path;
	for (shape = image->shapes; shape != NULL; shape = shape->next) {
		int ct = 0;
		for (path = shape->paths; path != NULL; path = path->next) {
			ct++;
			for (int i = 0; i < path->npts-1; i += 3) {
				float* p = &path->pts[i*2];
				CubicSpline tempSpline(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
				splines.push_back(tempSpline);
				cout <<" Bezer ctrl pts:" << p[0]<< " " << p[1]<< " " <<  p[2]<< " " << p[3]<< " " <<  p[4]<< " " << p[5]<< " " <<  p[6]<< " " << p[7] << endl;
				//drawCubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
			}
		}
		
			cout << ct << " bezier curves." << endl;
	}
	// Delete
	nsvgDelete(image);
}


int main(int argc, char* argv[])
{


	CubicSpline tempSpline(0.0f, 0.0f, 0.33f, 0.25f, 0.66f, 0.75f, 1.0f, 1.0f);
	//cout <<" Bezer ctrl pts:" << p[0]<< " " << p[1]<< " " <<  p[2]<< " " << p[3]<< " " <<  p[4]<< " " << p[5]<< " " <<  p[6]<< " " << p[7] << endl;


	loadSvg();
	Initialize(argc, argv);

	glutMainLoop();
	
	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	glutInit(&argc, argv);

	// Init Context	
	glutInitContextVersion(4, 0);
//	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	
	// Init Window
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if(WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}
	

	// set callbacks
	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0);
	glutCloseFunc(Cleanup);
	glutKeyboardFunc(KeyboardFunction);
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
		);
		exit(EXIT_FAILURE);
	}
	
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	// Init OpenGL
	// GRP: commented out so that polygon is filled initially
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_DEPTH_TEST);

	// Init Shaders and Vertices
	CreateShaders();
	CreateVBO();

	// Init Matrix	
	ModelMatrix = glm::mat4(1.0);	// Identity Matrix

	ViewMatrix = glm::lookAt(glm::vec3(0,0.0f,5.0f),	// eye
							glm::vec3(0,0,0.0f),	// center
							glm::vec3(0,1.0,0.0f)	// up
				);

    ProjectionMatrix = glm::perspective(45.0f, 3.0f / 3.0f, 0.1f, 100.f);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void KeyboardFunction(unsigned char Key, int X, int Y)
{
	X; Y; // Resolves warning C4100: unreferenced formal parameter

	switch (Key)
	{
	case '-':
		TessLevelInner = max(1.0f, TessLevelInner - 1.0f);
		break;
	case '=':
		TessLevelInner = min(32.0f, TessLevelInner + 1.0f);
		break;
	case '[':
		TessLevelOuter = max(1.0f, TessLevelOuter - 1.0f);
		break;
	case ']':
		TessLevelOuter = min(32.0f, TessLevelOuter + 1.0f);
		break;
    case 'd':
        displacement = !displacement;
        break;
    case 'w':
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        break;
    case 'f':
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        break;
    case ' ':
        animation = !animation;
        break;
	default:
		break;
	}
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction(void)
{
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// update Matrix
	glUniformMatrix4fv(ModelMatrixLocation, 1, GL_FALSE, &(ModelMatrix[0][0]));
	glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &(ViewMatrix[0][0]));
	glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

	// Draw the triangle
	// Starting from vertex 0; 6 vertices total -> 2 triangles
    glDrawArrays(GL_TRIANGLES, 0, 6);
	//dummy klm matrix	
	// glUniformMatrix4fv(klmLocation, 1, GL_FALSE, &(globalKlm[0][0]));

	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleFunction(void)
{
	glutPostRedisplay();
}

void TimerFunction(int Value)
{	
	FrameCount = 0;

	// update scene
	if(animation)
        ModelMatrix = glm::rotate(ModelMatrix, 1.0f, glm::vec3(1.0f,0,0));	// Rotate around x-axis
	
	// reset timer
	glutTimerFunc(10, TimerFunction, 1);
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void CreateVBO(void)
{
	Vertex Vertices[] =
	{
		// Note: for tex coords, UV (use only first two values) or KLM (use all 3)
		// - for UV, values are ALWAYS [0,0], [0.5, 0], and [1,1] RESPECTIVELY for 3 vertices (see Figure 25-2)
		// - for KLM, values need to be calculated (on CPU and pass it in or GPU)

		// Triangle #1:
		// vertex 0
		{
			{ 0.0f, 0.0f, 0.0f, 1.0f }, // position
			{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
			{ 0.0f, 0.0f, 0.0f }        // tex coords
		},
		{ {  1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.0f, 0.0f } }, // vertex 1		
		{ {  1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }, // vertex 2

		// Triangle #2
		{ {  1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // vertex 0 (same position as previous triangle 1's vertex 2, but with tex [0,0]
		{ {  2.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.0f, 0.0f } }, // vertex 1
		{ {  2.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }, // vertex 2
	};

	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].XYZW);
	const size_t TexOffset = sizeof(Vertices[0].XYZW) + sizeof(Vertices[0].RGBA);
	
	// Create Vertex Array Object
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	
	// Create Buffer for vertex data
	glGenBuffers(1, &BufferId);
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)TexOffset);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);  

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}

void DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &BufferId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}

GLuint LoadShader(const char* filename, GLenum shader_type)
{
	GLuint shader_id = 0;
	FILE* file;
	long file_size = -1;
	char* glsl_source;

	fprintf(stderr, "compiling %s.\n", filename);
	if (NULL != (file = fopen(filename, "rb")) &&
		0 == fseek(file, 0, SEEK_END) &&
		-1 != (file_size = ftell(file)))
	{
		rewind(file);
		
		if (NULL != (glsl_source = (char*)malloc(file_size + 1)))
		{
			if (file_size == (long)fread(glsl_source, sizeof(char), file_size, file))
			{
				glsl_source[file_size] = '\0';

				if (0 != (shader_id = glCreateShader(shader_type)))
				{
					const char *sources[] = {glsl_source}; 
					glShaderSource(shader_id, 1, sources, NULL);
					glCompileShader(shader_id);
					GLuint ErrorCheckValue = glGetError();
					if (ErrorCheckValue != GL_NO_ERROR)
					{
						fprintf(
							stderr,
							"ERROR: Could not compile a shader: %s \n",
							gluErrorString(ErrorCheckValue)
							);

						exit(-1);
					}

					//ExitOnGLError("Could not compile a shader");
				}
				else
					fprintf(stderr, "ERROR: Could not create a shader.\n");
			}
			else
				fprintf(stderr, "ERROR: Could not read file %s\n", filename);

			free(glsl_source);
		}
		else
			fprintf(stderr, "ERROR: Could not allocate %i bytes.\n", file_size);

		fclose(file);
	}
	else
		fprintf(stderr, "ERROR: Could not open file %s\n", filename);

	return shader_id;
}

void CreateShaders(void)
{
	GLchar log[2014];
	GLenum ErrorCheckValue = glGetError();

	VertexShaderId = LoadShader("shader.vs.glsl", GL_VERTEX_SHADER);	

	FragmentShaderId = LoadShader("shader.ps.glsl", GL_FRAGMENT_SHADER); 

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glLinkProgram(ProgramId);
	ErrorCheckValue = glGetError();

	if (ErrorCheckValue != GL_NO_ERROR)
	{
		glGetProgramInfoLog(ProgramId,1023,NULL,log);
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			log
			);
		
		exit(-1);
	}

	glUseProgram(ProgramId);

	ModelMatrixLocation = glGetUniformLocation(ProgramId, "ModelMatrix");
	ViewMatrixLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
	ProjectionMatrixLocation = glGetUniformLocation(ProgramId, "ProjectionMatrix");
    // template KLM matrix with the dummy values
	//CubicSpline tempSpline(0.0f, 0.0f, 0.33f, 0.25f, 0.66f, 0.75f, 1.0f, 1.0f);
	//globalKlm = tempSpline.KLM;
	//klmLocation = glGetUniformLocation(ProgramId, "klmMatrix");

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
			glGetProgramInfoLog(ProgramId,1023,NULL,log);
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			/*gluErrorString(ErrorCheckValue)*/
			log
		);

		exit(-1);
	}
}

void DestroyShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}
