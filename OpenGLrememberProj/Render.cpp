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

void Duck(double angle);

Point Norm(double A[3], double B[3], double C[3]);

void Osnova(int side, double saturation);
void Vert();
void Arc1(double A[][3], int side, double saturation);
void Arc2(double A[][3], int side, double saturation);
void NewVert(double angle, int Precision);
void drow_side(double A[][3], int firstPindex, int secondPinde, double angle, int textPindex, int Precision);

void Rott(double A[][3], int i, double angle);

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
{490, 662},		//3
{612, 815},		//3-
{241, 862},		//4-
{362, 1014},	//4
{367, 1222},	//4
{367, 1028},	//4-
{665, 1222},	//0-
{665, 1028},	//0
{940, 1477},	//0
{747, 1451},	//0-
{971, 1260},	//1-
{778, 1235},	//1
{973, 1250},	//1
{780, 1223},	//1-
{1023, 899},	//2-
{830, 874},		//2
{1232, 15},		//2
{1304, 196},	//2-
{602, 268},		//5-
{674, 447},		//5
{591, 272},		//5
{662, 452},		//5-
{345, 371},		//6-
{416, 551},		//6
{334, 375},		//6
{405, 555},		//6-
{16, 502},		//3-
{88, 682}};		//3

double OsnovTextPoints[][2] = {
	{674, 1016},
	{770, 1208},
	{819, 866},
	{624, 819},
	{377, 1016},
	{672, 461},
	{434, 557},
	{1284, 1016},
	{1189, 1207},
	{1139, 862},
	{1336, 820},
	{1580, 1015},
	{1287, 461},
	{1528, 557} };

double arc1[60][3] = { 0 };
double arc2[26][3] = { 0 };

double point[3] = { 0, 0, 4 };

bool f = false;
int change_angle = 0;
float ANGL = 0;
int Kof = 9, znak = -1;

bool textureMode = true;
bool lightMode = true;

bool alphaMode = false;
bool changetexture = false;
bool rememberchangetexture = true;
bool anglemod = false;
bool norm_from_the_cam = false;

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

	if (key == 'P')
	{
		change_angle++;
	}
	if (key == 'M')
	{
		change_angle--;
	}
	if (key == 'N')
	{
		change_angle = 0;
	}
	if (key == 'K')
	{
		anglemod = !anglemod;
	}
	if (key == 'Q')
	{
		norm_from_the_cam = !norm_from_the_cam;
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

	double angle = 0 + change_angle;
	if(anglemod)
		Duck(angle);
	else
		Duck((cos(ANGL) - sin(ANGL)) * Kof + angle);

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
	rec.setSize(300, 230);
	rec.setPosition(10, ogl->getHeight() - 230 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "C - сменить текстуру" << std::endl;
	ss << "Поворот: N(колебания)"<< std::endl << "M(по часовой); P(против часовой)" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "Q - изменить определение нормалей" << std::endl;
	if(norm_from_the_cam)
		ss << "Нормали в сторону камеры" << std::endl;
	else
		ss << "Нормали статичны" << std::endl;
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


void Duck(double angle)
{
	/*glTranslated(-2, -1, 0);*/

	glBindTexture(GL_TEXTURE_2D, changetexture ? texId[0]:texId[1]);

	Osnova(1, 1);
	NewVert(angle, 60);
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
	/*Point a = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
	Point b = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };
	
	double x = a.y * b.z - b.y * a.z;
	double y = -a.x * b.z + b.x * a.z;
	double z = a.x * b.y - b.y * a.y;*/
	
	double Ak = A[1] * (B[2] - C[2]) + B[1] * (C[2] - A[2]) + C[1] * (A[2] - B[2]);
	double Bk = A[2] * (B[0] - C[0]) + B[2] * (C[0] - A[0]) + C[2] * (A[0] - B[0]);
	double Ck = A[0] * (B[1] - C[1]) + B[0] * (C[1] - A[1]) + C[0] * (A[1] - B[1]);

	double x = Ak;
	double y = Bk;
	double z = Ck;

	if (norm_from_the_cam)
	{
		double Dk = A[0] * (B[1] * C[2] - C[1] * B[2]) + B[0] * (C[1] * A[2] - A[1] * C[2]) + C[0] * (A[1] * B[2] - B[1] * A[2]);
		Dk *= -1;
		double S = Ak * camera.pos.X() + Bk * camera.pos.Y() + Ck * camera.pos.Z() + Dk;
		if (S <= 0)
		{
			x *= -1;
			y *= -1;
			z *= -1;
		}
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
	glNormal3d((side&&!norm_from_the_cam)?-p.x:p.x, (side && !norm_from_the_cam) ? -p.y : p.y, (side && !norm_from_the_cam) ? -p.z : p.z);
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
	/*glNormal3d(p.x, p.y, p.z);*/
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

void NewVert(double angle, int Precision)
{
	Rott(DWN, 3, angle);
	Point p = Norm(DWN[3], point, DWN[4]);

	//glBindTexture(GL_TEXTURE_2D, texId);


	drow_side(DWN, 3, 4, angle, 0, Precision);
	drow_side(DWN, 4, 0, angle, 4, Precision);
	drow_side(DWN, 0, 1, angle, 8, Precision);
	drow_side(DWN, 1, 2, angle, 12, Precision);

	glBegin(GL_QUADS);

	for(int i = 0; i < 59; i++)
	{ 
		drow_side(arc1, i, i + 1, angle, 16, Precision);
	}

	drow_side(DWN, 5, 6, angle, 20, Precision);

	for (int i = 25; i > 0; i--)
	{
		drow_side(arc2, i, i - 1, angle, 24, Precision);
	}

	glEnd();

	_b = true;

}

void Rott_Point(Point A, double angle);

void drow_side(double A[][3], int firstPindex, int secondPindex, double angle, int textPindex, int Precision)
{
	double dangle = angle / Precision;
	double dz = (point[2] - A[firstPindex][2]) / Precision;
	int kof = 0;
	bool five_six = false;
	if (textPindex >= 16 && textPindex != 20)
		kof++;
	if (firstPindex == 5 && secondPindex == 6 && A == DWN)
		five_six = true;
	for (int i = 0; i < Precision; i++)
	{
		Rott_Point({A[firstPindex][0], A[firstPindex][1], A[firstPindex][2] + i * dz}, i * dangle);
		double pnt1[3] = { point[0], point[1], point[2] };
		Rott_Point({ A[firstPindex][0], A[firstPindex][1], A[firstPindex][2] + (i + 1) * dz }, (i + 1) * dangle);
		double pnt2[3] = { point[0], point[1], point[2] };
		Rott_Point({ A[secondPindex][0], A[secondPindex][1], A[secondPindex][2] + i * dz }, i * dangle);
		double pnt4[3] = { point[0], point[1], point[2] };
		Rott_Point({ A[secondPindex][0], A[secondPindex][1], A[secondPindex][2] + (i + 1) * dz }, (i + 1) * dangle);
		double pnt3[3] = { point[0], point[1], point[2] };

		double txtdx1 = (text(textPindex + 1, 0) - text(textPindex, 0)) / Precision;
		double txtdy1 = (text(textPindex + 1, 1) - text(textPindex, 1)) / Precision;
		double txtdx2 = (text(textPindex + 2, 0) - text(textPindex, 0)) / 60;
		double txtdy2 = (text(textPindex + 2, 1) - text(textPindex, 1)) / 60;
		
		Point p(pnt1);
		if(!five_six)
			p = Norm(pnt1, pnt2, pnt4);
		else
			p = Norm(pnt4, pnt1, pnt3);
		glBegin(GL_QUADS);

		glNormal3d((!norm_from_the_cam? -p.x : p.x), (!norm_from_the_cam? -p.y : p.y), (!norm_from_the_cam? -p.z : p.z));
		glColor3d(0.7, 0.7, 0.7);
		glTexCoord2d(text(textPindex, 0) + i * txtdx1 + kof * firstPindex * txtdx2, text(textPindex, 1) + i * txtdy1 + kof * firstPindex * txtdy2);
		glVertex3dv(pnt1);
		glTexCoord2d(text(textPindex, 0) + (i + 1) * txtdx1 + kof * firstPindex * txtdx2, text(textPindex, 1) + (i + 1) * txtdy1 + kof * firstPindex * txtdy2);
		glVertex3dv(pnt2);

		if(kof == 0)
		{ 
			glColor3d(0.6, 0.6, 0.6);
			glTexCoord2d(text(textPindex + 2, 0) + (i + 1) * txtdx1, text(textPindex + 2, 1) + (i + 1) * txtdy1);
			glVertex3dv(pnt3);
			glTexCoord2d(text(textPindex + 2, 0) + i * txtdx1, text(textPindex + 2, 1) + i * txtdy1);
			glVertex3dv(pnt4);
		}
		else
		{
			glColor3d(0.6, 0.6, 0.6);
			glTexCoord2d(text(textPindex, 0) + (i + 1) * txtdx1 + kof * secondPindex * txtdx2, text(textPindex, 1) + (i + 1) * txtdy1 + kof * secondPindex * txtdy2);
			glVertex3dv(pnt3);
			glTexCoord2d(text(textPindex, 0) + i * txtdx1 + kof * secondPindex * txtdx2, text(textPindex, 1) + i * txtdy1 + kof * secondPindex * txtdy2);
			glVertex3dv(pnt4);
		}

		glEnd();

		/*double P1[3] = { pnt1[0] + (!norm_from_the_cam ? (-p.x) : p.x), pnt1[1] + (!norm_from_the_cam ? (-p.y) : p.y), pnt1[2] + (!norm_from_the_cam ? (-p.z) : p.z) };
		double P2[3] = { pnt2[0] + (!norm_from_the_cam ? (-p.x) : p.x), pnt2[1] + (!norm_from_the_cam ? (-p.y) : p.y), pnt2[2] + (!norm_from_the_cam ? (-p.z) : p.z) };
		double P3[3] = { pnt3[0] + (!norm_from_the_cam ? (-p.x) : p.x), pnt3[1] + (!norm_from_the_cam ? (-p.y) : p.y), pnt3[2] + (!norm_from_the_cam ? (-p.z) : p.z) };
		double P4[3] = { pnt4[0] + (!norm_from_the_cam ? (-p.x) : p.x), pnt4[1] + (!norm_from_the_cam ? (-p.y) : p.y), pnt4[2] + (!norm_from_the_cam ? (-p.z) : p.z) };

		glLineWidth(2);
		glDisable(GL_LIGHTING);
		glColor3d(1, 0, 1);
		glBegin(GL_LINES);
		glVertex3dv(pnt1);
		glVertex3dv(P1);
		glVertex3dv(pnt2);
		glVertex3dv(P2);
		glVertex3dv(pnt3);
		glVertex3dv(P3);
		glVertex3dv(pnt4);
		glVertex3dv(P4);
		glEnd();

		glPointSize(10);
		glColor3d(1, 0, 0);
		glBegin(GL_POINTS);
		glVertex3dv(pnt1);
		glVertex3dv(P1);
		glVertex3dv(pnt2);
		glVertex3dv(P2);
		glVertex3dv(pnt3);
		glVertex3dv(P3);
		glVertex3dv(pnt4);
		glVertex3dv(P4);
		glEnd();
		glEnable(GL_LIGHTING);*/

	}

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
	/*glNormal3d(p.x, p.y, p.z);*/
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
	/*glNormal3d(p.x, p.y, -p.z);*/
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

void Rott(double A[][3], int i, double angle)
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

void Rott_Point(Point A, double angle)
{

	point[0] = A.x * cos(-M_PI * angle / 180) + A.y * sin(-M_PI * angle / 180);
	point[1] = -A.x * sin(-M_PI * angle / 180) + A.y * cos(-M_PI * angle / 180);
	point[2] = A.z;
}

double text(int k, int i)
{
	if (i == 0)
		return VertTextPoints[k][i] / 1610.;
	else
		return (1610 - VertTextPoints[k][i]) / 1610.;
}

double Newtext(double k, int i)
{
	if (i == 0)
		return k / 1610.;
	else
		return (1610 - k) / 1610.;
}

double text_o(int k, int i, int side)
{
	if (i == 0)
		return OsnovTextPoints[k + 7 * side][i] / 1610.;
	else
		return (1610 - OsnovTextPoints[k + 7 * side][i]) / 1610.;
}


//Point get_text_point(const Point& prev) {
//	return { (prev.x + 8) / 16, (prev.y - 9) / -16, prev.z };
//}
//
//Point get_part_of_text(const Point& part, const Point& begin_coor = { 0, 0.66, 1 }, double size = 0.33) {
//	auto tmp = get_text_point(part);
//	return { tmp.x * size + begin_coor.x, tmp.y * size + begin_coor.y, tmp.z };
//}

