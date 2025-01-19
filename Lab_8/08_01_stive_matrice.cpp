﻿//
// =================================================
// | Grafica pe calculator                          |
// =================================================
// | Laborator VIII - 08_01_stive_matrice.cpp       |
// =================================================
// 
//	Program ce deseneaza doua cuburi ce se deplaseaza.
//  Ambele cuburi se rotesc in jurul axei proprii, unul dintre cuburi se roteste in jurul celuilat
//	ELEMENTE DE NOUTATE:
//   - transformarile de modelare si cea de vizualizare sunt inglobate intr-o singura matrice;
//	 - folosirea stivelor de matrice;
//   - utilizarea timpului scurs de la initializare;
//   - in 08_01_Shader.frag: stabilirea culorii obiectului in functie de pozitia fragmentului
//
// 
//	Biblioteci
#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include<stack>


//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId,
EboId,
ProgramId,
viewModelLocation,
projLocation,
codColLocation;

//	Valaorea lui pi;
float PI = 3.141592;
// (1) intervalele pentru parametrii considerati (u si v)
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
// (2) numarul de paralele/meridiane, de fapt numarul de valori ptr parametri
int const NR_PARR = 10, NR_MERID = 20;
// (3) pasul cu care vom incrementa u, respectiv v
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

//	Dimensiunile ferestrei de afisare;
GLfloat
	winWidth = 1400, winHeight = 600;

//	Variabila ce determina schimbarea culorii pixelilor in shader;
int codCol;
float radius = 50;
int index, index_aux;

// Variabila pentru timpul scurs
float timeElapsed;

//	Elemente pentru matricea de vizualizare;
float obsX = 0.0, obsY = 0.0, obsZ = 300.f,
refX = 0.0f, refY = 0.0f, refZ = -100.f,
vX = 0.0;
//	Elemente pentru matricea de proiectie;
float xMin = -700.f, xMax = 700.f, yMin = -300.f, yMax = 300.f,
zNear = 100.f, zFar = 500.f,
width = 1400.f, height = 600.f, fov = 90.f * PI / 180;

//	Vectori pentru matricea de vizualizare;
glm::vec3
	obs, pctRef, vert;

//	Variabile catre matricile de transformare;
glm::mat4 
	view, projection,  
	translateSystem, 
	rotateSun, 
	scalePlanet, rotatePlanetAxis, rotatePlanet, translatePlanet;

// Stiva de matrice - inglobeaza matricea de modelare si cea de vizualizare
std::stack<glm::mat4> mvStack;

/// 1. Functii de desenare/conturare a corpurilor ceresti:
void DesenareObiect3D(const int culoare) {
	codCol = culoare;
	glUniform1i(codColLocation, codCol);

	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
}

void DesenareMuchii_Meridiane_Paralele(const int culoare, const float lineWidth) {
	codCol = culoare;
	glUniform1i(codColLocation, codCol);

	glLineWidth(lineWidth);

	// fiecare meridian separat
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		glDrawElements(
			GL_LINE_STRIP,
			NR_PARR + 1,
			GL_UNSIGNED_SHORT,
			(GLvoid*)((merid * NR_PARR + merid) * sizeof(GLushort)));
	}

	// o varianta mai simplista pt meridiane, in care apare si un segment Nord-Sud
	// glDrawElements(GL_LINE_STRIP, (NR_PARR + 1) * (NR_MERID), GL_UNSIGNED_SHORT, 0);

	// fiecare cerc paralel separat
	for (int parr = 1; parr < NR_PARR; parr++)
	{
		glDrawElements(
			GL_LINE_LOOP,
			NR_MERID,
			GL_UNSIGNED_SHORT,
			(GLvoid*)(((NR_PARR + 1) * (NR_MERID)+parr * NR_MERID) * sizeof(GLushort)));
	}
}
///

/// 1. Normal Keys: 'l', 'r', '+', '-';
void ProcessNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':			//	Apasarea tastelor `l` si `r` modifica pozitia verticalei in planul de vizualizare;
		vX += 0.1;
		break;
	case 'r':
		vX -= 0.1;
		break;
	case '+':			//	Apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
		obsZ += 10;
		break;
	case '-':
		obsZ -= 10;
		break;
	}
	if (key == 27)
		exit(0);
}

/// 1. Special Keys: 'LEFT', 'RIGHT', 'UP', 'DOWN';
void ProcessSpecialKeys(int key, int xx, int yy)
{
	switch (key)				//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	{							//	duce la deplasarea observatorului pe axele Ox si Oy;
	case GLUT_KEY_LEFT:
		obsX -= 20;
		break;
	case GLUT_KEY_RIGHT:
		obsX += 20;
		break;
	case GLUT_KEY_UP:
		obsY += 20;
		break;
	case GLUT_KEY_DOWN:
		obsY -= 20;
		break;
	}
}

//  Crearea si compilarea obiectelor de tip shader;
void CreateShaders(void)
{
	ProgramId = LoadShaders("08_01_Shader.vert", "08_01_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	//  Transmiterea datelor prin buffere;

	// (4) Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenBuffers(1, &VboId); // atribute
	glGenBuffers(1, &EboId); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;

	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	viewModelLocation = glGetUniformLocation(ProgramId, "viewModel");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	//	Realizarea proiectiei - pot fi utilizate si alte variante;
    projection = glm::ortho(xMin, xMax, yMin, yMax, zNear, zFar);
	//	projection = glm::frustum(xMin, xMax, yMin, yMax, zNear, zFar);
	//	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), zNear, zFar);
	//  projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), zNear); 
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//  Se curata ecranul OpenGL pentru a fi desenat noul continut (bufferul de culoare & adancime);
	glEnable(GL_DEPTH_TEST);                                //  Activarea testului de adancime

	/// 1. Variabila care indica timpul scurs de la initializare
	timeElapsed = glutGet(GLUT_ELAPSED_TIME);

	//	Matricea de vizualizare - actualizare
	//	Pozitia observatorului;
	obs = glm::vec3(obsX, obsY , obsZ);
	//	Pozitia punctului de referinta;
	refX = obsX; refY = obsY;
	pctRef = glm::vec3(refX, refY, refZ);
	//	Verticala din planul de vizualizare;
	vert = glm::vec3(vX, 1.0f, 0.0f);
	view = glm::lookAt(obs, pctRef, vert);
 
	/// 1. Matrice pentru miscarea obiectelor din sistem
	/// 
	/// Intregul sistem se deplaseaza prin translatie
	const float constSpeedX = 2.f;
	translateSystem = glm::translate(glm::mat4(1.0f), glm::vec3(-600 + constSpeedX * 0.01 * timeElapsed, 0.0, 0.0));
	
	// Soarele se roteste in jurul propriei axe
	rotateSun = glm::rotate(glm::mat4(1.0f), -(float)0.0001 * timeElapsed, glm::vec3(0.0, 1.0, 0.0));
	
	// Planeta se obtine scaland cubul initial
	scalePlanet = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.4));
	// Planeta se roteste in jurul propriei axe
	rotatePlanetAxis = glm::rotate(glm::mat4(1.0f), (float)0.001 * timeElapsed, glm::vec3(0.0, 1.0, 0.0));
	// Planeta se roteste in jurul astrului central
	rotatePlanet = glm::rotate(glm::mat4(1.0f), (float)0.0005 * timeElapsed, glm::vec3(-0.1, 1.0, 0.0));
	// Planeta este translatata in raport cu astrul central
	translatePlanet = glm::translate(glm::mat4(1.0f), glm::vec3(150.0, 0.0, 0.0));
	

	// Desenarea primitivelor + manevrarea stivei de matrice
	// 
	// Matricea de vizualizare este adaugata in varful stivei de matrice
	mvStack.push(view);                  // In varful stivei:   view 

	// 0. Pentru intregul sistem:
	// Matrice de translatie pentru intregul sistem
	mvStack.top() *= translateSystem;	 // In varful stivei:  view * translateSystem 
	mvStack.push(mvStack.top());         // Pe poz 2 a stivei: view * translateSystem 

	/// 1. Mini-sistem solar:
	/// I. Pentru SOARE (Astrul central)

	// Actualizare a matricei din varful stivei
	// Rotatie in jurul axei proprii
	mvStack.top() *= rotateSun;         // In varful stivei:  view * translateSystem * rotateSun          
	
	// Transmitere matrice de deplasare a Soarelui catre shader, apoi eliminare din varful stivei
	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.pop();		                 // In varful stivei:   view * translateSystem 

	/// 1. Desenarea propriu-zisa a obiectului 3D
	DesenareObiect3D(-1); /// Mix de culori pt. SOARE

	/// 1. Desenarea muchiilor - meridiane/paralele
	///  DesenareMuchii_Meridiane_Paralele(1, 2.f); // Black


	/// II. Pentru PAMANT (orbiteaza in jurul SOARELUI)
	
	// Actualizare a matricei din varful stivei
	// Rotatie in jurul Soarelui
	mvStack.top() *= rotatePlanet;		// In varful stivei:  view * translateSystem * rPl
	// Deplasare fata de centrul Soarelui
	mvStack.top() *= translatePlanet;   // In varful stivei:  view * translateSystem * rPl * tPl
	// Rotatie in jurul axei proprii
	mvStack.top() *= rotatePlanetAxis;  // In varful stivei:  view * translateSystem * rPl * tPl * rPlAx
	// Scalare (redimensionare obiect 3D)
	mvStack.top() *= scalePlanet;       // In varful stivei:  view * translateSystem * rPl * tPl * rPlAx * scPl

	// Transmitere matrice de deplasare a planetei catre shader, apoi eliminare din varful stivei
	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	
	/// 1. Desenarea propriu-zisa a obiectului 3D
	DesenareObiect3D(-2); /// Mix de culori pt. PAMANT

	/// 1. Desenarea muchiilor - meridiane/paralele
	///  DesenareMuchii_Meridiane_Paralele(1, 2.f); // Black


	/// III. Pentru LUNA (satelitul natural al PAMANTULUI, orbitand in jurul acestuia)

	// Actualizare a matricei din varful stivei
	// Rotatie in jurul Soarelui
	mvStack.top() *= rotatePlanet;		// In varful stivei:  view * translateSystem * rPl
	// Deplasare fata de centrul Soarelui
	mvStack.top() *= translatePlanet;   // In varful stivei:  view * translateSystem * rPl * tPl
	// Rotatie in jurul axei proprii
	mvStack.top() *= rotatePlanetAxis;  // In varful stivei:  view * translateSystem * rPl * tPl * rPlAx
	// Scalare (redimensionare obiect 3D)
	mvStack.top() *= scalePlanet;       // In varful stivei:  view * translateSystem * rPl * tPl * rPlAx * scPl

	// Transmitere matrice de deplasare a planetei catre shader, apoi eliminare din varful stivei
	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	/// 1. Desenarea propriu-zisa a obiectului 3D
	DesenareObiect3D(-3); /// Mix de culori pt. LUNA

	/// 1. Desenarea muchiilor - meridiane/paralele
	///  DesenareMuchii_Meridiane_Paralele(1, 2.f); // Black


	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata; 
	glFlush();			//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);		//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Miscare relativa. Utilizarea stivelor de matrice");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutKeyboardFunc(ProcessNormalKeys);	//	Functii ce proceseaza inputul de la tastatura utilizatorului;
	glutSpecialFunc(ProcessSpecialKeys);
	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}
