//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laborator VIII - 08_01_Shader.vert       |
// ===========================================
// 
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei; 
//

#version 330

//  Variabile de intrare (dinspre programul principal);
layout (location = 0) in vec4 in_Position;     //  Se preia din buffer de pe prima pozitie (0) atributul care contine coordonatele;
layout (location = 1) in vec3 in_Color;        //  Se preia din buffer de pe a doua pozitie (1) atributul care contine culoarea;

//  Variabile de iesire;
out vec4 gl_Position;   //  Transmite pozitia actualizata spre programul principal;
out vec3 ex_Color;      //  Transmite culoarea (de modificat in Shader.frag); 

/// 1. Transmite coordonata 'Y' normalizata (pozitia pe verticala) -> (de modificat in Shader.frag);
out float ex_normalizationY; // [yMin, yMax]

//  Variabile uniforme;
uniform mat4 viewModel;
uniform mat4 projection;

void main(void)
{
    gl_Position = projection * viewModel * in_Position;

    /// 1. Coordonata 'Y' normalizata in functie de pozitia pe verticala;
    ex_normalizationY = normalize(in_Position.y); // [yMin, yMax] -> [-1, 1]
    ///

    ex_Color = in_Color; 
}
 