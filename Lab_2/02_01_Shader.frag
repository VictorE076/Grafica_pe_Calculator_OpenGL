//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul II - 02_01_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core

in vec4 ex_Color;
out vec4 out_Color;

//	Variabilele uniforme;
uniform int codColShader;

//	Actualizeaza culoarea in functie de codColShader;
void main(void) {

/*
	if (codColShader == 0)
		out_Color = ex_Color; // Culorile in spectrul RGB ca atribute ale varfurilor (din fisierul cpp): "GLfloat Colors[] = {......}";
	else
		out_Color = vec4 (1.0, 0.0, 1.0, 0.0); // Purple (Magenta -> Default)
*/

	switch(codColShader)
	{
		case 0:
			out_Color = vec4 (1.0, 1.0, 0.0, 1.0); // Yellow (for lines)
			break;

		case 1:
			out_Color = vec4 (1.0, 0.5, 0.0, 1.0); // Orange (for vertices)
			break;

		case 2:
			out_Color = vec4 (0.5, 1.0, 1.0, 1.0); // Cyan (for vertices)
			break;

		case 3:
			out_Color = vec4 (0.0, 1.0, 0.0, 1.0); // Green (for vertices)
			break;

		default:
			break;
	}

}
 