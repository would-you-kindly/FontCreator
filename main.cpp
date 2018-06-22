#include <stdlib.h>
#include <GL/glui.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>

int window_id;
int bitSize = 50;
int hBitsCount = 8; // 8, 16, 24, 32 только
int vBitsCount = 8;
bool bits[32][32]; // заранее выделяем больше места, чтобы можно было изменять размер битовой карты

std::ofstream fout;

GLUI_EditText* bitsSizeEditText;
GLUI_RadioGroup* hBitsRadioGroup;
GLUI_EditText* vBitsEditText;
GLUI_EditText* fontEditText;
GLUI_EditText* letterEditText;
GLUI_Button* addLetterButton;

void printBits(std::ostream& out)
{
	if (typeid(out) == typeid(fout))
	{
		fout << std::hex << std::showbase << letterEditText->get_text() << "   { ";
		int value = 0;
		for (int j = 0; j < vBitsCount; j++) // идем с начала, т.к. битовые карты рисуются с левого нижнего угла
		{
			for (int i = 0; i < hBitsCount; i++) // идем по количеству битов в строке (которых должно быть 8, 16, 24, 32)
			{
				value += bits[i][j] * pow(2, 8 - 1 - (i % 8));
				if (i % 8 == 8 - 1)
				{
					fout << std::hex << std::showbase << value;
					if (!(i + 1 == hBitsCount && j + 1 == vBitsCount))
						fout << ", ";
					value = 0;
				}
			}
		}
		fout << " };" << std::endl;
	}
	if (typeid(out) == typeid(std::cout))
	{
		system("cls");
		int value = 0;
		for (int j = vBitsCount - 1; j >= 0; j--) // идем с конца, чтобы записывать сначала верхние строки битов, потом нижние
		{
			for (int i = 0; i < hBitsCount; i++) // идем по количеству битов в строке (которых должно быть 8, 16, 24, 32)
			{
				value += bits[i][j] * pow(2, 8 - 1 - (i % 8));
				if (i % 8 == 8 - 1)
				{
					std::cout << std::hex << std::showbase << value;
					if (i + 1 != hBitsCount) // если не последнее значение
						std::cout << '\t';
					value = 0;
				}
			}
			std::cout << std::endl;
		}
	}
}

void drawGrid()
{
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = 0; i < hBitsCount; i++)
		for (int j = 0; j < vBitsCount; j++)
		{
			glBegin(GL_LINE_LOOP);
			glVertex2i(i*bitSize, j*bitSize);
			glVertex2i(i*bitSize + bitSize, j*bitSize);
			glVertex2i(i*bitSize + bitSize, j*bitSize + bitSize);
			glVertex2i(i*bitSize, j*bitSize + bitSize);
			glEnd();
		}
	glPopAttrib();
}

void drawBits()
{
	// здесь просто проходим по всем битам, поэтому неважно, 
	// идти сначала по строкам или столбцам
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.f, 1.f, 1.f);
	for (int i = 0; i < hBitsCount; i++)
		for (int j = 0; j < vBitsCount; j++)
			if (bits[i][j])
			{
				glBegin(GL_QUADS);
				glVertex2i(i*bitSize, j*bitSize);
				glVertex2i(i*bitSize + bitSize, j*bitSize);
				glVertex2i(i*bitSize + bitSize, j*bitSize + bitSize);
				glVertex2i(i*bitSize, j*bitSize + bitSize);
				glEnd();
			}
	glPopAttrib();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	printBits(std::cout);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLfloat)w, 0.0, (GLfloat)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	drawBits();
	drawGrid();

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button != GLUT_MIDDLE_BUTTON)
	{
		int bitX, bitY;
		bitX = x / bitSize;
		bitY = vBitsCount - 1 - y / bitSize;
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			bits[bitX][bitY] = true;
			break;
		case GLUT_RIGHT_BUTTON:
			bits[bitX][bitY] = false;
			break;
		}
		printBits(std::cout);
	}
}

void idle()
{
	glutSetWindow(window_id);
	glutPostRedisplay();
}

void closeApplication(int id)
{
	exit(0);
}

void clearBits(int id)
{
	for (int i = 0; i < hBitsCount; i++)
		for (int j = 0; j < vBitsCount; j++)
			bits[i][j] = false;
	printBits(std::cout);
}

void fillBits(int id)
{
	for (int i = 0; i < hBitsCount; i++)
		for (int j = 0; j < vBitsCount; j++)
			bits[i][j] = true;
	printBits(std::cout);
}

void changeBitsSize(int id)
{
	bitSize = bitsSizeEditText->get_int_val();
	glutReshapeWindow(bitSize * hBitsCount, bitSize * vBitsCount);
}

void changeHBits(int id)
{
	switch (hBitsRadioGroup->get_int_val())
	{
	case 0: hBitsCount = 8; break;
	case 1: hBitsCount = 16; break;
	case 2: hBitsCount = 24; break;
	case 3: hBitsCount = 32; break;
	}
	glutReshapeWindow(bitSize * hBitsCount, bitSize * vBitsCount);
	printBits(std::cout);
}

void changeVBits(int id)
{
	vBitsCount = vBitsEditText->get_int_val();
	glutReshapeWindow(bitSize * hBitsCount, bitSize * vBitsCount);
	printBits(std::cout);
}

void addLetter(int id)
{
	if (!fout.is_open())
		fout.open((std::string)fontEditText->get_text() + ".txt",
		std::ios_base::app | std::ios_base::ate | std::ios_base::out); // добавляем к имеющемуся в файле, в конец файла, пишем в файл
	printBits(fout);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(bitSize * hBitsCount, bitSize * vBitsCount);
	window_id = glutCreateWindow("Font Creator");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	GLUI_Master.set_glutIdleFunc(idle);

	GLUI* glui = GLUI_Master.create_glui("GLUI");

	GLUI_Panel* bitmapSizePanel = new GLUI_Panel(glui, "Bitmap size");
	bitsSizeEditText = new GLUI_EditText(bitmapSizePanel, "Bits size", GLUI_EDITTEXT_INT, 0, changeBitsSize);
	bitsSizeEditText->set_int_val(bitSize);
	bitsSizeEditText->set_int_limits(25, 50);
	GLUI_Panel* hBitsPanel = new GLUI_Panel(bitmapSizePanel, "Bits columns");
	hBitsRadioGroup = new GLUI_RadioGroup(hBitsPanel, (int*)0, 0, changeHBits);
	new GLUI_RadioButton(hBitsRadioGroup, "8");
	new GLUI_RadioButton(hBitsRadioGroup, "16");
	new GLUI_RadioButton(hBitsRadioGroup, "24");
	new GLUI_RadioButton(hBitsRadioGroup, "32");
	vBitsEditText = new GLUI_EditText(bitmapSizePanel, "Bits strings", GLUI_EDITTEXT_INT, 0, changeVBits);
	vBitsEditText->set_int_limits(8, 32);

	new GLUI_Button(glui, "Clear", 0, clearBits); // пока очищается только видимая область окна (отмеченные в большом окне биты не очищаются)
	new GLUI_Button(glui, "Fill", 0, fillBits); // тут также (возможно можно оставить и так)

	GLUI_Panel* creatingFontsPanel = new GLUI_Panel(glui, "Creating fonts");
	fontEditText = new GLUI_EditText(creatingFontsPanel, "Font", GLUI_EDITTEXT_TEXT);
	letterEditText = new GLUI_EditText(creatingFontsPanel, "Letter");
	addLetterButton = new GLUI_Button(creatingFontsPanel, "Add letter", 0, addLetter);
	new GLUI_Button(creatingFontsPanel, "Delete letter");

	//new GLUI_Button(glui, "Load letter"); // загрузить существующую букву из существующего шрифта из списков
	//new GLUI_Listbox(glui, "Fonts");
	//new GLUI_Listbox(glui, "Letters"); // если можно обновлять динамически список букв после выбора определенного шрифта, то сделать так, иначе убрать listbox'ы


	//new GLUI_Button(glui, "Print hex", 0, printHex); // можно без этой кнопки, вместо этого каждый раз при изменении картинки переписывать консоль
	new GLUI_Button(glui, "Exit", 0, closeApplication);

	glui->set_main_gfx_window(window_id);

	glutMainLoop();

	fout.close();
	return 0;
}