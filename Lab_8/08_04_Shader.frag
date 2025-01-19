//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul VIII - 08_04_Shader.frag|
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core

//	Variabile de intrare (dinspre Shader.vert);
in vec3 ex_Color;

//	Variabile de iesire	(spre programul principal);
out vec3 out_Color;

//  Variabile uniforme;
uniform int codCol;

void main(void)
{
    switch (codCol)
    {
        case 1: // Black
            out_Color = vec3(0.0, 0.0, 0.0); 
            break;
        /// 3.
        case -1: // Green
            out_Color = vec3(0.0, 1.0, 0.0);
            break;
        case -2: // Brown
            out_Color = vec3(0.55, 0.27, 0.07);
            break;
        ///
        default: 
            out_Color = ex_Color;
            break;
    }
}