#include "Render.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


#include <Windows.h>
#include <cmath>
#define _USE_MATH_DEFINES 
#include <math.h>

class Point
{
public: Point(double A, double B, double C)
{
	x = A;
	y = B; 
	z = C;
}
	Point(double* pnt)
	{
		x = pnt[0];
		y = pnt[1];
		z = pnt[2];
	}

	operator double*()
	{
		double ret[3] = { x, y, z };
		return ret;
	}

	  double x, y, z;
};

void Duck(int angle);

Point Norm(double A[3], double B[3], double C[3]);

void Osnova(int side, double saturation);
void Vert();
void Arc1(double A[][3], int side, double saturation);
void Arc2(double A[][3], int side, double saturation);
void NewVert(int angle);
void Rott(double A[][3], int i, int angle);

double text(int k, int i);
double text_o(int k, int i, int side);

//Point get_text_point(const Point& prev);
//Point get_part_of_text(const Point& part, const Point& begin_coor = { 0, 0.66, 1 }, double size = 0.33);

double UP[][3] = {
	{ 1, -4, 0 },
	{ 3, -8, 0 },
	{ 4, -1, 0 },
	{ 0, 0, 0 },
	{ -5, -4, 0 },
	{ 1, 7, 0 },
	{ -4, 5, 0 } };

double DWN[][3] = {
	{ -1, -5, 0 },
	{ 1, -9, 0 },
	{ 2, -2, 0 },
	{ -2, -1, 0 },
	{ -7, -5, 0 },
	{ -1, 6, 0 },
	{ -6, 4, 0 } };

double VertTextPoints[][2] = {
{490, 662},
{612, 815},
{241, 862},
{362, 1014},
{367, 1222},
{367, 1028},
{665, 1222},
{665, 1028},
{940, 1477},
{747, 1451},
{971, 1260},
{778, 1235},
{973, 1250},
{780, 1223},
{1023, 899},
{830, 874},
{1232, 15},
{1304, 196},
{602, 268},
{674, 447},
{591, 272},
{662, 452},
{345, 371},
{416, 551},
{334, 375},
{405, 555},
{16, 502},
{88, 682}};

double OsnovTextPoints[][2] = {
	{674, 1016},
	{770, 1208},
	{819, 866},
	{624, 819},
	{377, 1016},
	{672, 461},
	{434, 557},
	{1289, 1021},
	{1186, 1228},
	{1835, 868},
	{1341, 816},
	{1598, 1021},
	{1289, 456},
	{1547, 559} };

double arc1[60][3] = { 0 };
double arc2[26][3] = { 0 };

double point[3] = { 0, 0, 4 };

float f = false;

float ANGL = 0;
int Kof = 9, znak = -1;

bool textureMode = true;
bool lightMode = true;

bool alphaMode = false;
bool changetexture = false;
bool rememberchangetexture = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'A')
	{
		alphaMode = !alphaMode;
	}

	if (key == 'C')
	{
		changetexture = !changetexture;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}


//
//GLuint texId;

//выполняется перед первым рендером
//void initRender(OpenGL *ogl)
//{
//	//настройка текстур
//
//	//4 байта на хранение пикселя
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
//
//	//настройка режима наложения текстур
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//
//	////включаем текстуры
//	//glEnable(GL_TEXTURE_2D);
//	//
//
//	////массив трехбайтных элементов  (R G B)
//	//RGBTRIPLE *texarray;
//
//	////массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
//	//char *texCharArray;
//	//int texW, texH;
//	//OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray);
//	//OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
//
//	//
//	//
//	////генерируем ИД для текстуры
//	//glGenTextures(1, &texId);
//	////биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
//	//glBindTexture(GL_TEXTURE_2D, texId);
//
//	////загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
//
//	////отчистка памяти
//	//free(texCharArray);
//	//free(texarray);
//
//	////наводим шмон
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//
//	//камеру и свет привязываем к "движку"
//	ogl->mainCamera = &camera;
//	ogl->mainLight = &light;
//
//	// нормализация нормалей : их длины будет равна 1
//	glEnable(GL_NORMALIZE);
//
//	// устранение ступенчатости для линий
//	glEnable(GL_LINE_SMOOTH); 
//
//
//	//   задать параметры освещения
//	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
//	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
//	//                1 - лицевые и изнаночные обрабатываются разными режимами       
//	//                соответственно лицевым и изнаночным свойствам материалов.    
//	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
//	//                не зависящее от сточников
//	// по умолчанию (0.2, 0.2, 0.2, 1.0)
//
//	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
//
//	camera.fi1 = -1.3;
//	camera.fi2 = 0.8;
//}

GLuint texId[2];

//выполняется перед первым рендером
void initRender(OpenGL* ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов (R G B)
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4 4, потомучто выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	RGBTRIPLE* texarray2;
	char* texCharArray2;
	int texW2, texH2;
	OpenGL::LoadBMP("texture2.bmp", &texW2, &texH2, &texarray2);
	OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);

	//генерируем ИД для текстуры
	glGenTextures(2, texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	//загружаем текстуру в видеопямять, в оперативке нам больше она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	//загружаем текстуру в видеопямять, в оперативке нам больше она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);
	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//отчистка памяти
	free(texCharArray);
	free(texarray);
	free(texCharArray2);
	free(texarray2);

	//OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray);
	//OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	////генерируем ИД для текстуры
	//glGenTextures(1, );
	////биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	//glBindTexture(GL_TEXTURE_2D, texID2);

	////загружаем текстуру в видеопямять, в оперативке нам больше она не нужна
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	////отчистка памяти
	//free(texCharArray);
	//free(texarray);

	////наводим шмон
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	// задать параметры освещения
	// параметр GL_LIGHT_MODEL_TWO_SIDE -
	// 0 - лицевые и изнаночные рисуются одинаково(по умолчанию),
	// 1 - лицевые и изнаночные обрабатываются разными режимами
	// соответственно лицевым и изнаночным свойствам материалов.
	// параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение,
	// не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}




void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	/*
	if (changetexture != rememberchangetexture)
	{
		rememberchangetexture = changetexture;

		//=============================================================================================
		//включаем текстуры
		glEnable(GL_TEXTURE_2D);


		//массив трехбайтных элементов  (R G B)
		RGBTRIPLE* texarray;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray;
		int texW, texH;
		OpenGL::LoadBMP(changetexture ? "texture1.bmp" : "texture2.bmp", &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//генерируем ИД для текстуры
		glGenTextures(1, &texId);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, texId);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//отчистка памяти
		free(texCharArray);
		free(texarray);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//=============================================================================================
	}*/

	

	Duck((cos(ANGL) - sin(ANGL)) * Kof);

	ANGL += M_PI / 9;
	if (ANGL >= M_PI * 2)
	{
		Kof += znak;
		ANGL = 0;

		if (Kof == 10 || Kof == 0)
			znak *= -1;
	}

	////Начало рисования квадратика станкина
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	//glBindTexture(GL_TEXTURE_2D, texId);

	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);

	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	////конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 170);
	rec.setPosition(10, ogl->getHeight() - 170 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "C - сменить текстуру" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "A - вкл/выкл прозрачность" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}


void Duck(int angle)
{
	/*glTranslated(-2, -1, 0);*/

	glBindTexture(GL_TEXTURE_2D, changetexture ? texId[0]:texId[1]);

	Osnova(1, 1);
	NewVert(angle);
	glTranslated(0, 0, 4);
	glRotated(angle, 0, 0, 1);
	if(alphaMode)
	{ 
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Osnova(0, 0.4);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}
	else
		Osnova(0, 1);
}

Point Norm(double * A, double * B, double * C)
{
	Point a = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
	Point b = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };
	

	double x = a.y * b.z - b.y * a.z;
	double y = -a.x * b.z + b.x * a.z;
	double z = a.x * b.y - b.y * a.y;


	double Ak = A[1] * (B[2] - C[2]) + B[1] * (C[2] - A[2]) + C[1] * (A[2] - B[2]);
	double Bk = A[2] * (B[0] - C[0]) + B[2] * (C[0] - A[0]) + C[2] * (A[0] - B[0]);
	double Ck = A[0] * (B[1] - C[1]) + B[0] * (C[1] - A[1]) + C[0] * (A[1] - B[1]);
	double Dk = A[0] * (B[1] * C[2] - C[1] * B[2]) + B[0] * (C[1] * A[2] - A[1] * C[2]) + C[0] * (A[1] * B[2] - B[1] * A[2]);
	Dk *= -1;
	double S = Ak * camera.pos.X() + Bk * camera.pos.Y() + Ck * camera.pos.Z() + Dk;
	if (S <= 0)
	{
		x *= -1;
		y *= -1;
		z *= -1;
	}
	double l = sqrt(x * x + y * y + z * z);
	x /= l;
	y /= l;
	z /= l;

	return {x,y,z};
}

void Osnova(int side, double saturation)
{
	Point p = Norm(DWN[0], DWN[1], DWN[2]);


	glBegin(GL_TRIANGLE_FAN); 
	glColor4d(0.8, 0.8, 0.8, saturation);
	glNormal3d(p.x, p.y, p.z);
	glTexCoord2d(text_o(0, 0, side), text_o(0, 1, side));
	glVertex3dv(DWN[0]);
	glTexCoord2d(text_o(1, 0, side), text_o(1, 1, side));
	glVertex3dv(DWN[1]);
	glTexCoord2d(text_o(2, 0, side), text_o(2, 1, side));
	glVertex3dv(DWN[2]);
	glTexCoord2d(text_o(3, 0, side), text_o(3, 1, side));
	glVertex3dv(DWN[3]);
	glTexCoord2d(text_o(4, 0, side), text_o(4, 1, side));
	glVertex3dv(DWN[4]);
	glEnd();

	p = Norm(DWN[3], DWN[2], DWN[5]);

	glBegin(GL_TRIANGLES);
	glNormal3d(p.x, p.y, p.z);
	glColor4d(0.8, 0.8, 0.8, saturation);
	glTexCoord2d(text_o(3, 0, side), text_o(3, 1, side));
	glVertex3dv(DWN[3]);
	glTexCoord2d(text_o(2, 0, side), text_o(2, 1, side));
	glVertex3dv(DWN[2]);
	glTexCoord2d(text_o(5, 0, side), text_o(5, 1, side));
	glVertex3dv(DWN[5]);

	//glVertex3dv(DWN[3]);
	//glVertex3dv(DWN[6]);
	//glVertex3dv(DWN[5]);
	glEnd();

	Arc1(DWN, side, saturation);
	Arc2(DWN, side, saturation);

	/*double eye[][3] = {
		{1.5, 3.8, 0.05},
		{3, 3, 0.05},
		{1.5, 4, 0.05},
		{0, 4, 0.05} };

	glBegin(GL_TRIANGLE_FAN);
	glColor4d(0.2, 0.2, 0.2, saturation);
	glVertex3dv(eye[0]);
	glVertex3dv(eye[1]);
	glVertex3dv(eye[2]);
	glVertex3dv(eye[3]);
	glEnd();*/
}

void Vert()
{
	glBegin(GL_QUAD_STRIP);
	glColor3d(0.5, 0.4, 0.7);
	glVertex3dv(DWN[0]);
	glVertex3dv(UP[0]);
	glColor3d(0.4, 0.3, 0.6);
	glVertex3dv(DWN[1]);
	glVertex3dv(UP[1]);
	glColor3d(0.5, 0.4, 0.7);
	glVertex3dv(DWN[2]);
	glVertex3dv(UP[2]);
	glColor3d(0.4, 0.3, 0.6);
	glVertex3dv(DWN[5]);
	glVertex3dv(UP[5]);
	glColor3d(0.5, 0.4, 0.7);
	glVertex3dv(DWN[6]);
	glVertex3dv(UP[6]);
	glColor3d(0.4, 0.3, 0.6);
	glVertex3dv(DWN[3]);
	glVertex3dv(UP[3]);
	glColor3d(0.5, 0.4, 0.7);
	glVertex3dv(DWN[4]);
	glVertex3dv(UP[4]);
	glColor3d(0.4, 0.3, 0.6);
	glVertex3dv(DWN[0]);
	glVertex3dv(UP[0]);
	glEnd();

}


bool _b = false;

void NewVert(int angle)
{
	Rott(DWN, 3, angle);
	Point p = Norm(DWN[3], point, DWN[4]);

	//glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_QUAD_STRIP);

	glNormal3d(p.x, p.y, p.z);
	glColor3d(0.7, 0.7, 0.7);
	glTexCoord2d(text(0,0), text(0,1));
	glVertex3dv(DWN[3]);
	glTexCoord2d(text(1, 0), text(1, 1));
	glVertex3dv(point);
	Rott(DWN, 4, angle);
	p = Norm(DWN[4], point, DWN[0]);
	glColor3d(0.6, 0.6, 0.6);
	glTexCoord2d(text(2, 0), text(2, 1));
	glVertex3dv(DWN[4]);
	glTexCoord2d(text(3, 0), text(3, 1));
	glVertex3dv(point);
	glNormal3d(p.x, p.y, p.z);
	glTexCoord2d(text(4, 0), text(4, 1));
	glVertex3dv(DWN[4]);
	glTexCoord2d(text(5, 0), text(5, 1));
	glVertex3dv(point);
	Rott(DWN, 0, angle);
	p = Norm(DWN[0], point, DWN[1]);
	glColor3d(0.7, 0.7, 0.7);
	glTexCoord2d(text(6, 0), text(6, 1));
	glVertex3dv(DWN[0]);
	glTexCoord2d(text(7, 0), text(7, 1));
	glVertex3dv(point);
	glNormal3d(p.x, p.y, p.z);
	glTexCoord2d(text(8, 0), text(8, 1));
	glVertex3dv(DWN[0]);
	glTexCoord2d(text(9, 0), text(9, 1));
	glVertex3dv(point);
	Rott(DWN, 1, angle);
	p = Norm(DWN[1], point, DWN[2]);
	glColor3d(0.6, 0.6, 0.6);
	glTexCoord2d(text(10, 0), text(10, 1));
	glVertex3dv(DWN[1]);
	glTexCoord2d(text(11, 0), text(11, 1));
	glVertex3dv(point);
	glNormal3d(p.x, p.y, p.z);
	glTexCoord2d(text(12, 0), text(12, 1));
	glVertex3dv(DWN[1]);
	glTexCoord2d(text(13, 0), text(13, 1));
	glVertex3dv(point);
	Rott(DWN, 2, angle);
	p = Norm(DWN[2], point, arc1[0]);
	glColor3d(0.7, 0.7, 0.7);
	glTexCoord2d(text(14, 0), text(14, 1));
	glVertex3dv(DWN[2]);
	glTexCoord2d(text(15, 0), text(15, 1));
	glVertex3dv(point);
	glNormal3d(p.x, p.y, p.z);
	glTexCoord2d(text(16, 0), text(16, 1));
	glVertex3dv(DWN[2]);
	glTexCoord2d(text(17, 0), text(17, 1));
	glVertex3dv(point);

	for (int i = 0; i < 60; i++)
	{
		if(i<59)
		{ 
			double P[1][3] = {arc1[i][0], arc1[i][1], arc1[i][2] + 4};
			Rott(P, 0, angle);
			p = Norm(arc1[i], point, arc1[i+1]);
		}
		else
		{
		}

		glNormal3d(p.x, p.y, p.z);

		double dx1 = (text(18, 0) - text(16, 0)) / 60;
		double dy1 = (text(18, 1) - text(16, 1)) / 60;
		double dx2 = (text(19, 0) - text(17, 0)) / 60;
		double dy2 = (text(19, 1) - text(17, 1)) / 60;


		glColor3d(0.6, 0.6, 0.6);
		glTexCoord2d(text(16, 0) + i * dx1, text(16, 1) + i * dy1);
		glVertex3dv(arc1[i]);
		arc1[i][2] += 4;
		glColor3d(0.7, 0.7, 0.7);
		Rott(arc1, i, angle);
		glTexCoord2d(text(17, 0) + i * dx2, text(17, 1) + i * dy2);
		glVertex3dv(point);
		arc1[i][2] -= 4;

	}

	Rott(DWN, 5, angle);
	p = Norm(DWN[5], point, DWN[6]);
	glNormal3d(p.x, p.y, p.z);
	glColor3d(0.7, 0.7, 0.0);
	glTexCoord2d(text(20, 0), text(20, 1));
	glVertex3dv(DWN[5]);
	glTexCoord2d(text(21, 0), text(21, 1));
	glVertex3dv(point);

	Rott(DWN, 6, angle);
	glTexCoord2d(text(22, 0), text(22, 1));
	glVertex3dv(DWN[6]);
	glTexCoord2d(text(23, 0), text(23, 1));
	glVertex3dv(point);

	
	std::stringstream str_strm;
	

	for (int i = 25; i >= 0; i--)
	{
		if (i > 0)
		{
			double P[1][3] = { arc2[i][0], arc2[i][1], arc2[i][2] + 4 };
			Rott(P, 0, angle);
			p = Norm(arc2[i], point, arc2[i - 1]);
			str_strm <<std::setprecision(3) << "x = " <<  p.x << " y = " << p.y << " z = " << p.z << std::endl;
			if (!_b)
			 OutputDebugString(str_strm.str().c_str());
		}


		double dx1 = (text(26, 0) - text(24, 0)) / 60;
		double dy1 = (text(26, 1) - text(24, 1)) / 60;
		double dx2 = (text(27, 0) - text(25, 0)) / 60;
		double dy2 = (text(27, 1) - text(25, 1)) / 60;

		glNormal3d(p.x, p.y, p.z);
		glColor3d(0.6, 0.6, 0.0);
		glTexCoord2d(text(24, 0) + i * dx1, text(24, 1) + i * dy1);
		glVertex3dv(arc2[i]);
		arc2[i][2] += 4;
		glColor3d(0.7, 0.7, 0.0);
		Rott(arc2, i, angle);
		glTexCoord2d(text(25, 0) + i * dx2, text(25, 1) + i * dy2);
		glVertex3dv(point);
		arc2[i][2] -= 4;
	}


	glEnd();
	_b = true;

}

void Arc1(double A[][3], int side, double saturation)
{
	double O[] = { (A[2][0] + A[5][0]) / 2, (A[2][1] + A[5][1]) / 2, 0 };
	double r = sqrt(pow(A[2][0] - A[5][0], 2) + pow(A[2][1] - A[5][1], 2)) / 2;
	arc1[0][0] = A[2][0];
	arc1[0][1] = A[2][1];
	arc1[0][2] = A[2][2];
	for (int i = 1; i < 59; i++)
	{
		arc1[i][0] = O[0] + r * cos(M_PI / 60 * (i - 23));
		arc1[i][1] = O[1] + r * sin(M_PI / 60 * (i - 23));
		arc1[i][2] = arc1[0][2];
	}
	arc1[59][0] = A[5][0];
	arc1[59][1] = A[5][1];
	arc1[59][2] = A[5][2];

	Point p = Norm(arc1[0], arc1[1], arc1[2]);

	glBegin(GL_TRIANGLE_FAN);
	glColor4d(0.8, 0.8, 0.8, saturation);
	glNormal3d(p.x, p.y, p.z);
	for (int i = 0; i < 60; i++)
	{
		double dx = (text_o(5, 0, side) - text_o(2, 0, side)) * (arc1[i][0] - A[2][0]) / (A[5][0] - A[2][0]);
		double dy = (text_o(5, 1, side) - text_o(2, 1, side)) * (arc1[i][1] - A[2][1]) / (A[5][1] - A[2][1]);

		glTexCoord2d(text_o(2, 0, side) + dx, text_o(2, 1, side) + dy);
		glVertex3dv(arc1[i]);
	}
	glEnd();
}

void Arc2(double A[][3], int side, double saturation)
{
	double a = -4 - A[6][0],
		b = 3 - A[6][1],
		c = A[3][0] - A[6][0],
		d = A[3][1] - A[6][1];
	double e = a * (A[6][0] - 4) + b * (A[6][1] + 3),
		f = c * (A[6][0] + A[3][0]) + d * (A[3][1] + A[6][1]),
		g = 2 * (a * (A[3][1] - 3) - b * (A[3][0] + 4));


	double O[] = { (d * e - b * f) / g, (a * f - c * e) / g, 0 };
	double r = sqrt(pow(A[3][0] - O[0], 2) + pow(A[3][1] - O[1], 2));
	arc2[0][0] = A[3][0];
	arc2[0][1] = A[3][1];
	arc2[0][2] = A[3][2];
	for (int i = 1; i < 25 /*arc2[i][0] > DWN[6][0] && arc2[i][1] < DWN[6][1]*/; i++)
	{
		arc2[i][0] = O[0] + r * cos(M_PI / 60 * (i + 1));
		arc2[i][1] = O[1] + r * sin(M_PI / 60 * (i + 1));
		arc2[i][2] = arc2[0][2];
	}
	arc2[25][0] = A[6][0];
	arc2[25][1] = A[6][1];
	arc2[25][2] = A[6][2];

	Point p = Norm(arc2[0], arc2[1], arc2[2]);

	glBegin(GL_TRIANGLE_FAN);
	glColor4d(0.7, 0.7, 0, saturation);
	glNormal3d(p.x, p.y, -p.z);
	glTexCoord2d(text_o(5, 0, side), text_o(5, 1, side));
	glVertex3dv(A[5]);
	for (int i = 0; i < 26; i++)
	{
		double dx = (text_o(5, 0, side) - text_o(2, 0, side)) * (arc2[i][0] - A[2][0]) / (A[5][0] - A[2][0]);
		double dy = (text_o(5, 1, side) - text_o(2, 1, side)) * (arc2[i][1] - A[2][1]) / (A[5][1] - A[2][1]);

		glTexCoord2d(text_o(2, 0, side) + dx, text_o(2, 1, side) + dy);
		glVertex3dv(arc2[i]);
	}
	glEnd();
}

void Rott(double A[][3], int i, int angle)
{

	point[0] = A[i][0] * cos(-M_PI * angle / 180) + A[i][1] * sin(-M_PI * angle / 180);
	point[1] = -A[i][0] * sin(-M_PI * angle / 180) + A[i][1] * cos(-M_PI * angle / 180);

	//point[0] -= 2;
	//point[1] -= 1;

	/*double r = sqrt(pow(point[0], 2) + pow(point[1], 2));
	double d = sqrt(pow(r - point[0], 2) + pow(-point[1], 2));
	double angl = acos((pow(d, 2) - 2 * pow(r, 2)) / (2 * pow(r, 2)));
	point[0] = -r * cos(M_PI * angle / 180 + angl);
	point[1] = -r * sin(M_PI * angle / 180 + angl);*/

}

double text(int k, int i)
{
	if (i == 0)
		return VertTextPoints[k][i] / 1610.;
	else
		return (1610 - VertTextPoints[k][i]) / 1610.;
}

double text_o(int k, int i, int side)
{
	if (i == 0)
		return OsnovTextPoints[k /*+ 7 * side*/][i] / 1610.;
	else
		return (1610 - OsnovTextPoints[k /*+ 7 * side*/][i]) / 1610.;
}


//Point get_text_point(const Point& prev) {
//	return { (prev.x + 8) / 16, (prev.y - 9) / -16, prev.z };
//}
//
//Point get_part_of_text(const Point& part, const Point& begin_coor = { 0, 0.66, 1 }, double size = 0.33) {
//	auto tmp = get_text_point(part);
//	return { tmp.x * size + begin_coor.x, tmp.y * size + begin_coor.y, tmp.z };
//}

