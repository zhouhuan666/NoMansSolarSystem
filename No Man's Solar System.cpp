#include <GL/glut.h>
#include <cstdlib>
#include <map>
#include <string>
#include <ctime>
#include <functional>

/// ���ǿ��Զ༶�����������ƽ�������
// ���Ǹ߶ȣ�Y�ᣩ���ƫ�ƻ�ʹ��̫��ϵ������ʵһЩ��ÿ������Ĺ��ƽ�治����ȫ�غ�
// �����ɿ������ӹ������ƫ�ơ���б������ʹ����Բ���
// �˴����Ǹ�����ʵ�ʴ�����ɫȷ��


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

	GLfloat planetRadius = 3;		// ����뾶
	GLint planeSlices = 20;			// ��γ��
	GLint planeStacks = 16;			// ��γ��

	GLfloat oribitRadius = 0;		// ����뾶
	GLfloat oribitSpeed = 2;		// �����ٶ�
	GLfloat faceSpeed = 2;			// �Դ��ٶ�
	GLcolor planetcolor = 0XFFFFFF;	// ��ɫ
	GLfloat oribitAngle = 0;		// ��ת�Ƕ�
	GLfloat faceAngle = 0;			// ��ת�Ƕ�

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
	Sun["����"] = Planet(0.8f, 16, 10, 10.0f, 10, 10, 0X007ACC, rand() % 360, rand() % 360, rand() % 200 / 100.f - 1.f);
	Sun["����"]["����"] = Planet(0.3, 16, 10, 1.5f, -5, rand() % 40 - 20, 0XDFDFDF, rand() % 360, rand() % 360);

	// ��ʼ��OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(120, 100);
	glutCreateWindow(R"(NO MAN'S SKY)");

	glutDisplayFunc(onWindowDsiplay);
	glutReshapeFunc(onWindowReshape);
	glutKeyboardFunc(onKeyboard);
	glutTimerFunc(20, onTimerFlushWindow, Planet::IDT_FLUSHWINDOW);

	// �������Ȩ
	glutMainLoop();
	return 0;
}

void onWindowDsiplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();  //���ص�λ����  
	gluLookAt(0, 0, 20, 0.0, 0, 0.0, 0.0, 1.0, 0);
	glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
	glRotatef(xAngle, 1.0, 0.0, 0.0);
	glRotatef(yAngle, 0.0, 1.0, 0.0);
	glRotatef(zAngle, 0.0, 0.0, 1.0);

	std::function<void(const Planet&)> drawPlanet;
	drawPlanet = [&drawPlanet](const Planet& planet)
	{
		// ��ת
		glRotatef(planet.oribitAngle, 0, 1, 0);
		glTranslatef(planet.oribitRadius, 0, 0);
		glRotatef(planet.oribitAngle, 0, -1, 0);		// �ָ��Ƕ�

		for (auto& pMap : planet)
			drawPlanet(pMap.second);

		glRotatef(planet.oribitAngle, 0, 1, 0);		// 
		// ��ת
		glRotatef(planet.faceAngle, 0, 1, 0);

		glColor3f(PlaceRGBF(planet.planetcolor));
		glutWireSphere(planet.planetRadius, planet.planeSlices, planet.planeStacks);
		// ��ת�ָ�
		glRotatef(planet.faceAngle, 0, -1, 0);

		// ��ת�ָ�
		glTranslatef(-planet.oribitRadius, 0, 0);
		glRotatef(planet.oribitAngle, 0, -1, 0);	// �ָ�������
	};

	// �ݹ����
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
