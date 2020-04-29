#include <GL/glut.h>
#include <cstdlib>
#include <map>
#include <string>
#include <ctime>
#include <functional>

/// 行星可以多级创建，由名称建立索引
// 行星高度（Y轴）轨道偏移会使得太阳系更加真实一些，每个星球的轨道平面不是完全重合
// 后续可考虑增加轨道中心偏移、倾斜，甚至使用椭圆轨道
// 八大行星根据其实际大致颜色确定


#ifndef RGB
using DWORD = unsigned int;
using COLORREF = DWORD;
using WORD = unsigned short;
using BYTE = unsigned char;
constexpr COLORREF RGB(BYTE r, BYTE g, BYTE b) { return ((COLORREF)(b | (g << 8)) | (r << 16)); }
constexpr COLORREF GRAY(BYTE c) { return RGB(c, c, c); }
constexpr BYTE GetRValue(COLORREF rgb) { return rgb >> 16; }
constexpr BYTE GetGValue(COLORREF rgb) { return rgb >> 8; }
constexpr BYTE GetBValue(COLORREF rgb) { return rgb; }
#define PlaceRGBF(rgb) GetRValue(rgb)/255., GetGValue(rgb)/255., GetBValue(rgb)/255.
#endif // ! RGB

struct Planet
	: public std::map<std::string, Planet>
{
	using GLcolor = unsigned int;

	enum :int { IDT_FLUSHWINDOW };

	GLfloat planetRadius = 3;		// 星球半径
	GLint planeSlices = 20;			// 经纬线
	GLint planeStacks = 16;			// 经纬线

	GLfloat oribitRadius = 0;		// 轨道半径
	GLfloat oribitSpeed = 2;		// 运行速度
	GLfloat faceSpeed = 2;			// 自传速度
	GLcolor planetcolor = 0XFFFFFF;	// 颜色
	GLfloat oribitAngle = 0;		// 公转角度
	GLfloat faceAngle = 0;			// 自转角度

	Planet(GLfloat planetRadius, GLint planeSlices, GLint planeStacks
		, GLfloat oribitRadius, GLfloat oribitSpeed
		, GLfloat faceSpeed, GLcolor planetcolor
		, GLfloat oribitAngle, GLfloat faceAngle
		, GLfloat oribitOffsetY = 0, bool showOrbit = true)
		: planetRadius(planetRadius)
		, planeSlices(planeSlices)
		, oribitRadius(oribitRadius)
		, oribitSpeed(oribitSpeed)
		, faceSpeed(faceSpeed)
		, planetcolor(planetcolor)
		, oribitAngle(oribitAngle)
		, faceAngle(faceAngle)
	{}

	Planet() = default;
	Planet(const Planet&) = default;
	Planet(Planet&&) = default;
	~Planet() = default;
	Planet& operator=(const Planet&) = default;
	Planet& operator=(Planet&&) = default;

	void moveNext()
	{
		if (oribitSpeed != 0)
		{
			if (oribitRadius > 0)
			{
				oribitAngle += oribitSpeed / oribitRadius;

				while (oribitAngle >= 360)
					oribitAngle -= 360;
				while (oribitAngle < 0)
					oribitAngle += 360;
			}
		}
		if (faceSpeed != 0)
		{
			if (planetRadius > 0)
			{
				faceAngle += faceSpeed / planetRadius;

				while (faceAngle >= 360)
					faceAngle -= 360;
				while (faceAngle < 0)
					faceAngle += 360;
			}
		}

		for (auto& p : *this)
			p.second.moveNext();
	}
};
Planet Sun;
GLfloat rateRotation = 0.0;
GLfloat rotationAngle = 0.0;
GLfloat xAngle = 0.0;
GLfloat yAngle = 0.0;
GLfloat zAngle = 0.0;
GLint screenWidth = 800;
GLint screenHeight = 600;
GLfloat oldMouseX;
GLfloat oldMouseY;

void onWindowDsiplay(void);
void onWindowReshape(int w, int h);
void onKeyboard(unsigned char key, int x, int y);
void onTimerFlushWindow(int);

int main(int argc, char** argv)
{
	srand(time(0));

	Sun = Planet(2, 30, 16, 0, 0, 0, 0XDD001B, 0, 0, 0, false);
	Sun["地球"] = Planet(0.8f, 16, 10, 10.0f, 10, 10, 0X007ACC, rand() % 360, rand() % 360, rand() % 200 / 100.f - 1.f);
	Sun["地球"]["月球"] = Planet(0.3, 16, 10, 1.5f, -5, rand() % 40 - 20, 0XDFDFDF, rand() % 360, rand() % 360);

	// 初始化OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(120, 100);
	glutCreateWindow(R"(NO MAN'S SKY)");

	glutDisplayFunc(onWindowDsiplay);
	glutReshapeFunc(onWindowReshape);
	glutKeyboardFunc(onKeyboard);
	glutTimerFunc(20, onTimerFlushWindow, Planet::IDT_FLUSHWINDOW);

	// 交予控制权
	glutMainLoop();
	return 0;
}

void onWindowDsiplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();  //加载单位矩阵  
	gluLookAt(0, 0, 20, 0.0, 0, 0.0, 0.0, 1.0, 0);
	glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
	glRotatef(xAngle, 1.0, 0.0, 0.0);
	glRotatef(yAngle, 0.0, 1.0, 0.0);
	glRotatef(zAngle, 0.0, 0.0, 1.0);

	std::function<void(const Planet&)> drawPlanet;
	drawPlanet = [&drawPlanet](const Planet& planet)
	{
		// 公转
		glRotatef(planet.oribitAngle, 0, 1, 0);
		glTranslatef(planet.oribitRadius, 0, 0);
		glRotatef(planet.oribitAngle, 0, -1, 0);		// 恢复角度

		for (auto& pMap : planet)
			drawPlanet(pMap.second);

		glRotatef(planet.oribitAngle, 0, 1, 0);		// 
		// 自转
		glRotatef(planet.faceAngle, 0, 1, 0);

		glColor3f(PlaceRGBF(planet.planetcolor));
		glutWireSphere(planet.planetRadius, planet.planeSlices, planet.planeStacks);
		// 自转恢复
		glRotatef(planet.faceAngle, 0, -1, 0);

		// 公转恢复
		glTranslatef(-planet.oribitRadius, 0, 0);
		glRotatef(planet.oribitAngle, 0, -1, 0);	// 恢复父环境
	};

	// 递归绘制
	glPushMatrix();
	drawPlanet(Sun);
	glPopMatrix();

	glutSwapBuffers();
}

void onWindowReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 5.0, 180);
	glMatrixMode(GL_MODELVIEW);
}

void onKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void onTimerFlushWindow(int id)
{
	if (id != Planet::IDT_FLUSHWINDOW)
		return;
	Sun.moveNext();
	glutPostRedisplay();
	return glutTimerFunc(20, onTimerFlushWindow, Planet::IDT_FLUSHWINDOW);
}
