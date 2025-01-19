//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laborator VIII - 08_01_Shader.frag       |
// ===========================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330

float winWidth = 1400; /// 1. Latimea ferestrei
//	Variabile de intrare (dinspre Shader.vert);
in vec4 gl_FragCoord; /// 1. Variabila care indica pozitia fragmentului (prin raportare la fereastra de vizualizare)
in vec3 ex_Color;
/// 1. Coordonata 'Y' normalizata (pozitia pe verticala);
in float ex_normalizationY; // [-1, 1]
//	Variabile de iesire	(spre programul principal);
out vec3 out_Color;
uniform int codCol; // Variabila uniforma

float lerp = (1 + ex_normalizationY) * 0.5f; // [0, 1]

void main(void)
{
    switch (codCol)
    {
        case 1: // Black
            out_Color = vec3(0.0, 0.0, 0.0); 
            break;
        case 2:
            out_Color = vec3(gl_FragCoord.x / winWidth, 0.0, 0.0); // Culoarea este stabilita in functie de pozitia fragmentului in fereastra 
            break;
        case -1: /// Mix de culori pt. SOARE (warmYellow, orange)
            out_Color = mix(vec3(1., 0.85, 0.), vec3(1., 0.5, 0.), lerp);
            break;
        case -2: /// Mix de culori pt. PAMANT (cyan, lightGreen)
            out_Color = mix(vec3(0., 1., 1.), vec3(0.2, 0.8, 0.2), lerp);
            break;
        case -3: /// Mix de culori pt. LUNA (lightGray, darkGray)
            out_Color = mix(vec3(0.85, 0.85, 0.85), vec3(0.75, 0.75, 0.75), lerp);
            break;
        default: 
            out_Color = ex_Color;
            break;
    }
}