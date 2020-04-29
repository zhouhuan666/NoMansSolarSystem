#include <GL/glut.h>
#include <cstdlib>
#include <map>
#include <string>
#include <ctime>
#include <functional>
#include <mutex>

/// ����
// ����϶� �޸��ӽ�
// P ��ͣ
// L �ı�����������
// D ��С̫������뾶
// U ����̫������뾶
// S ��ʾ/����̫�����
// O ��ʾ/�������й��
// C ��ʾ�������������
// R �������������ת

/// ���ǿ��Զ༶�����������ƽ�������
// ���Ǹ߶ȣ�Y�ᣩ���ƫ�ƻ�ʹ��̫��ϵ������ʵһЩ��ÿ������Ĺ��ƽ�治����ȫ�غ�
// �����ɿ������ӹ������ƫ�ơ���б������ʹ����Բ���
// �˴����Ǹ�����ʵ�ʴ�����ɫȷ��

namespace thatboy
{
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
		GLfloat oribitOffsetY = 0;		// Y������ƫ��

		bool showOrbit = true;			// �Ƿ���ƹ��

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
			, oribitOffsetY(oribitOffsetY)
			, showOrbit(showOrbit)
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

	namespace SolarSystem
	{
		Planet Sun;

		namespace PlanetControl
		{
			// ����
			bool ifShowCube = false;
			bool ifShowOrbit = true;
			bool ifOrbitLine = true;
			bool ifAutoRun = true;
			bool ifOribitRotate = true;

			GLfloat rateRotation = 0.0;
			GLfloat rotationAngle = 0.0;
			GLfloat xAngle = 0.0;
			GLfloat yAngle = 0.0;
			GLfloat zAngle = 0.0;
			GLint screenWidth = 1920;
			GLint screenHeight = 1080;
			GLfloat oldMouseX;
			GLfloat oldMouseY;

			void onWindowDsiplay(void);
			void onWindowReshape(int w, int h);
			void onKeyboard(unsigned char key, int x, int y);
			void onTimerFlushWindow(int);
			void onMouseMsg(int button, int state, int x, int y);
			void onMotion(int x, int y);
		}
	}

}

int main(int argc, char** argv)
{
	srand(time(0));

	using thatboy::Planet;
	using thatboy::SolarSystem::Sun;

	// �������
	Sun = Planet(3, 30, 16, 8.f, -2, -1, 0XDD001B, 0, 0, 0, false);// ������Ļ����ת��
	Sun["����"] = Planet(1.2f, 16, 10, 15.0f, 10, 10, 0X007ACC, rand() % 360, rand() % 360, rand() % 200 / 100.f - 1.f);

	// ��Ӷ������ǣ����ǣ����������ǡ��ļ�����
	Sun["����"]["����"] = Planet(0.8, 16, 10, 3.0f, -5, rand() % 40 - 20, 0XDFDFDF, rand() % 360, rand() % 360);

	// ��ʼ��OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(R"(NO MAN'S SKY)");
	glutFullScreen();
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	glutDisplayFunc(thatboy::SolarSystem::PlanetControl::onWindowDsiplay);
	glutReshapeFunc(thatboy::SolarSystem::PlanetControl::onWindowReshape);
	glutKeyboardFunc(thatboy::SolarSystem::PlanetControl::onKeyboard);
	glutTimerFunc(20, thatboy::SolarSystem::PlanetControl::onTimerFlushWindow, Planet::IDT_FLUSHWINDOW);
	glutMouseFunc(thatboy::SolarSystem::PlanetControl::onMouseMsg);
	glutMotionFunc(thatboy::SolarSystem::PlanetControl::onMotion);

	// �������Ȩ
	glutMainLoop();
	return 0;
}

void thatboy::SolarSystem::PlanetControl::onWindowDsiplay(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();  //���ص�λ����  
	gluLookAt(38, 50, 38, 0.0, -20.0, 0.0, 0.0, 1.0, 0);
	glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
	glRotatef(xAngle, 1.0, 0.0, 0.0);
	glRotatef(yAngle, 0.0, 1.0, 0.0);
	glRotatef(zAngle, 0.0, 0.0, 1.0);

	auto drawCircle = [](int x, int y, int r, float startAngle = 0)
	{
		int n = 180;
		constexpr GLfloat PI = 3.141592654;
		n = sqrt(r) * 18;
		glRotatef(startAngle, 0, 1, 0);
		glBegin(ifOrbitLine ? GL_LINES : GL_POLYGON);
		for (int i = 0; i < n; i++)
		{
			GLfloat curAnlge = 2 * PI / n * i;
			glVertex3f(x + r * cos(curAnlge), 0, y + r * sin(curAnlge));
		}
		glEnd();
		glRotatef(startAngle, 0, -1, 0);
	};

	std::function<void(const Planet&)> drawPlanet;
	drawPlanet = [&drawPlanet, &drawCircle](const Planet& planet)
	{
		// ���ƹ��(���ڸ�������ϵ��
		if (ifShowOrbit && planet.showOrbit)
		{
			glTranslatef(0, planet.oribitOffsetY, 0);
			glColor3f(PlaceRGBF(planet.planetcolor));
			drawCircle(0, 0, planet.oribitRadius, ifOribitRotate ? planet.oribitAngle : 0);
			glTranslatef(0, -planet.oribitOffsetY, 0);
		}
		// ��ת
		glRotatef(planet.oribitAngle, 0, 1, 0);
		glTranslatef(planet.oribitRadius, 0, 0);
		glRotatef(planet.oribitAngle, 0, -1, 0);		// �ָ��Ƕ�

		glTranslatef(0, planet.oribitOffsetY, 0);
		for (auto& pMap : planet)
			drawPlanet(pMap.second);
		glTranslatef(0, -planet.oribitOffsetY, 0);

		glRotatef(planet.oribitAngle, 0, 1, 0);		// 
		// ��ת
		glRotatef(planet.faceAngle, 0, 1, 0);

		// ���ƫ��
		glTranslatef(0, planet.oribitOffsetY, 0);

		glColor3f(PlaceRGBF(planet.planetcolor));
		if (ifShowCube)
			glutWireCube(planet.planetRadius);
		else
			glutWireSphere(planet.planetRadius, planet.planeSlices, planet.planeStacks);
		// ���ƫ�ƻָ�
		glTranslatef(0, -planet.oribitOffsetY, 0);
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

void thatboy::SolarSystem::PlanetControl::onWindowReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 5.0, 180);
	glMatrixMode(GL_MODELVIEW);
}

void thatboy::SolarSystem::PlanetControl::onKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
	case 'P':
		ifAutoRun = !ifAutoRun;
		break;
	case 'l':
	case 'L':
		ifOrbitLine = !ifOrbitLine;
		break;
	case 'd':
	case 'D':
		if (Sun.oribitRadius > 0)
			Sun.oribitRadius -= 0.1;
		else
			Sun.oribitRadius = 0;
		break;
	case 'u':
	case 'U':
		Sun.oribitRadius += 0.1;
		break;
	case 's':
	case 'S':
		Sun.showOrbit = !Sun.showOrbit;
		break;
	case 'o':
	case 'O':
		ifShowOrbit = !ifShowOrbit;
		break;
	case 'c':
	case 'C':
		ifShowCube = !ifShowCube;
		break;
	case 'r':
	case 'R':
		ifOribitRotate = !ifOribitRotate;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void thatboy::SolarSystem::PlanetControl::onTimerFlushWindow(int id)
{
	if (id != Planet::IDT_FLUSHWINDOW)
		return;
	if (ifAutoRun)
		Sun.moveNext();
	glutPostRedisplay();
	return glutTimerFunc(20, onTimerFlushWindow, Planet::IDT_FLUSHWINDOW);
}

void thatboy::SolarSystem::PlanetControl::onMouseMsg(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rateRotation = 0;
			rotationAngle = 0;
			oldMouseX = x;
			oldMouseY = y;
		}
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rateRotation += 1.0f;
		}
	}
}

void thatboy::SolarSystem::PlanetControl::onMotion(int x, int y)
{
	GLint deltax = oldMouseX - x;
	GLint deltay = oldMouseY - y;
	xAngle += 360 * static_cast<GLfloat>(deltax) / screenWidth;
	yAngle += 360 * static_cast<GLfloat>(deltay) / screenHeight;
	zAngle += 360 * static_cast<GLfloat>(deltay) / screenHeight;
	oldMouseX = x;
	oldMouseY = y;
	glutPostRedisplay();
}
