//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul X - 10_01_modele3D.cpp |
// =============================================
// 
// Program care deseneaza un model 3D importat  

// Biblioteci
#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdio.h>
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <cstdlib> 
#include <vector>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "loadShaders.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/// 1.
#include "objloader.hpp"
///

//  Identificatorii obiectelor de tip OpenGL;
GLuint
  VaoId1, VaoId2, 
  VboId1, VboId2,
  EboId1, EboId2, 
  ProgramId,
  nrVertLocation,
  myMatrixLocation,
  viewLocation,
  projLocation,
  codColLocation;

//	Elemente pentru matricea de vizualizare;
float refX = 0.0f, refY = 0.0f, refZ = 0.0f,
obsX, obsY, obsZ,
vX = 0.0f, vY = 0.0f, vZ = 1.0f;

//	Elemente pentru deplasarea pe sfera;
float alpha = 0.0f, beta = 0.0f, dist = 6.0f,
incrAlpha1 = 0.01, incrAlpha2 = 0.01;

//	Elemente pentru matricea de proiectie;
float width = 800, height = 600, dNear = 4.f, fov = 60.f * PI / 180, zNear = 1.f;

/// 1. Elemente pentru reprezentarea suprafetei unei SFERE si a unui CILINDRU:
const float PI = 3.141592f;
const float U_MIN_SFERA = -PI / 2, U_MAX_SFERA = PI / 2;
const float V_MIN_SFERA = 0, V_MAX_SFERA = 2 * PI;
const int NR_PARR = 10, NR_MERID = 20;
float step_u_SFERA = (U_MAX_SFERA - U_MIN_SFERA) / NR_PARR;
float step_v_SFERA = (V_MAX_SFERA - V_MIN_SFERA) / NR_MERID;
float radius_SFERA = 100;

const float U_MIN_CILINDRU = -PI, U_MAX_CILINDRU = PI;
const float V_MIN_CILINDRU = 0, V_MAX_CILINDRU = 90 * PI;
float step_u_CILINDRU = (U_MAX_CILINDRU - U_MIN_CILINDRU) / NR_PARR;
float step_v_CILINDRU = (V_MAX_CILINDRU - V_MIN_CILINDRU) / NR_MERID;
float radius_CILINDRU = 40;

// alte variabile
int codCol;
int index, index_aux;

// pentru fereastra de vizualizare 
GLint winWidth = 1000, winHeight = 600;

// vectori
glm::vec3 Obs, PctRef, Vert;

// Variabila pentru numarul de varfuri
int nrVertices;

// Vectori pentru varfuri, coordonate de texturare, normale
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

// Matrice utilizate
glm::mat4 myMatrix;
glm::mat4 view;
glm::mat4 projection;

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case '-':
		dist -= 0.25;	//	apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
		break;
	case '+':
		dist += 0.25;
		break;
	}
	if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)				//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	{							//	duce la deplasarea observatorului pe suprafata sferica in jurul cubului;
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;
	case GLUT_KEY_UP:
		alpha += incrAlpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incrAlpha1 = 0.f;
		}
		else
		{
			incrAlpha1 = 0.01f;
		}
		break;
	case GLUT_KEY_DOWN:
		alpha -= incrAlpha2;
		if (abs(alpha + PI / 2) < 0.05)
		{
			incrAlpha2 = 0.f;
		}
		else
		{
			incrAlpha2 = 0.01f;
		}
		break;
	}
}

/*
// Se initializeaza un vertex Buffer Object(VBO) pentru transferul datelor spre memoria placii grafice(spre shadere);
// In acesta se stocheaza date despre varfuri;
void CreateVBO(void)
{

// Generare VAO;
  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);

 // Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER, VboId);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), &normals[0]);

  // Atributele; 
  glEnableVertexAttribArray(0); // atributul 0 = pozitie
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  glEnableVertexAttribArray(1); // atributul 1 = normale
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vertices.size() * sizeof(glm::vec3)));

}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VaoId);  
}
*/

//  Crearea si compilarea obiectelor de tip shader;
void CreateShaders(void)
{
	ProgramId = LoadShaders("10_01_Shader.vert", "10_01_Shader.frag");
	glUseProgram(ProgramId);
}

/// 3. COPAC (Coroana + Trunchi), folosind 2 VAO-uri:
void CreateVAO1(void)
{
	/// 3. SFERA (Coroana Copacului)
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices1[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors1[(NR_PARR + 1) * NR_MERID];
	GLushort Indices1[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN_SFERA + parr * step_u_SFERA; // valori pentru u si v
			float v = V_MIN_SFERA + merid * step_v_SFERA;
			float x_vf = radius_SFERA * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius_SFERA * cosf(u) * sinf(v);
			float z_vf = radius_SFERA * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices1[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors1[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices1[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices1[(NR_PARR + 1) * NR_MERID + index_aux] = index;

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
				Indices1[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices1[AUX + 4 * index + 1] = index2;
				Indices1[AUX + 4 * index + 2] = index3;
				Indices1[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId1);
	glBindVertexArray(VaoId1);
	glGenBuffers(1, &VboId1); // atribute
	glGenBuffers(1, &EboId1); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1) + sizeof(Colors1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices1), sizeof(Colors1), Colors1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1), Indices1, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices1)));
}

void CreateVAO2(void)
{
	/// 3. CILINDRU (Trunchiul Copacului)
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices2[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors2[(NR_PARR + 1) * NR_MERID];
	GLushort Indices2[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN_CILINDRU + parr * step_u_CILINDRU; // valori pentru u si v
			float v = V_MIN_CILINDRU + merid * step_v_CILINDRU;
			float x_vf = radius_CILINDRU * cosf(u); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius_CILINDRU * sinf(u);
			float z_vf = v;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors2[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices2[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices2[(NR_PARR + 1) * NR_MERID + index_aux] = index;

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
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId2);
	glBindVertexArray(VaoId2);
	glGenBuffers(1, &VboId2); // atribute
	glGenBuffers(1, &EboId2); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2)));
}

// Elimina obiectele de tip shader dupa rulare;
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
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId2);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
};

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(0.95f, 0.82f, 0.4f, 1.0f); // culoarea de fond a ecranului
 
	/// 1. Incarcarea modelului 3D in format OBJ, trebuie sa fie in acelasi director cu proiectul actual;
	///  bool model = loadOBJ("model_blender.obj", vertices, uvs, normals);
	bool model = loadOBJ("High Poly.obj", vertices, uvs, normals);
	nrVertices = vertices.size();

	// Crearea VBO / shadere-lor
	/// CreateVBO();

	CreateVAO1();
	CreateVAO2();

	CreateShaders();

	// Locatii ptr shader
	nrVertLocation = glGetUniformLocation(ProgramId, "nrVertices");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	// Variabile ce pot fi transmise catre shader
	glUniform1i(ProgramId, nrVertices);
}

//	Functie utilizata in setarea matricelor de vizualizare si proiectie;
void SetMVP(void)
{  
   // Matricea de modelare 
   myMatrix = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 1.0, 0.0))
	   * glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 0.0, 1.0));
   glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

   //	Vizualizare;
   //	Pozitia observatorului - se deplaseaza pe sfera;
   obsX = refX + dist * cos(alpha) * cos(beta);
   obsY = refY + dist * cos(alpha) * sin(beta);
   obsZ = refZ + dist * sin(alpha);
   //	Vectori pentru matricea de vizualizare;
   glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);		//	Pozitia observatorului;	
   glm::vec3 pctRef = glm::vec3(refX, refY, refZ); 	//	Pozitia punctului de referinta;
   glm::vec3 vert = glm::vec3(vX, vY, vZ);			//	Verticala din planul de vizualizare; 
   // Matricea de vizualizare, transmitere catre shader
   view = glm::lookAt(obs, pctRef, vert);
   glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

   //	Proiectie;
   projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), dNear);
   glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

   /*
   // "Legarea"VAO, desenare;
   glBindVertexArray(VaoId);
   glEnableVertexAttribArray(0); // atributul 0 = pozitie
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
   */

   // glDrawArrays(GL_TRIANGLES, 0, vertices.size()*sizeof(GLfloat));
   
   glutSwapBuffers();
   glFlush ( );
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	// Initializare ecran + test de adancime;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	SetMVP();

	/// 3. Desenare COPAC:
	/// SFERA (Coroana Copacului)
	glBindVertexArray(VaoId1);
	codCol = -1; /// Green;
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

	/// CILINDRUL (Trunchiul Copacului)
	glBindVertexArray(VaoId2);
	codCol = -2; /// Brown;
	glUniform1i(codColLocation, codCol);
	///  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (GLvoid*)(0));
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	glutSwapBuffers();
	glFlush();
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
  glutInitWindowPosition (100,100); 
  glutInitWindowSize(1200,900); 
  glutCreateWindow("Utilizarea unui model predefinit in format OBJ");
  glewInit(); 
  Initialize();
  glutDisplayFunc(RenderFunction);
  glutIdleFunc(RenderFunction);
  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(processSpecialKeys);
  glutCloseFunc(Cleanup);
  glutMainLoop();
}

