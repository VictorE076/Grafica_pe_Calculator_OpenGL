//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IX - 09_02f_Shader.vert |
// =======================================
// 
//  Shaderul de varfuri; 

#version 330 core

//  Variabile de intrare (dinspre programul principal);
layout(location=0) in vec3 in_Position;                 //  Se preia din buffer de pe prima pozitie (0) atributul care contine coordonatele;
layout(location=1) in vec3 in_Normal;                   //  Se preia din buffer de pe a doua pozitie (1) atributul care contine normala;

//  Variabile de iesire;
out vec4 gl_Position; 
out vec3 Normal;
out vec3 FragPos;
out vec3 inLightPos;
out vec3 inViewPos;

//  Variabile uniforme;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(void)
{
    gl_Position = projection * view * vec4(in_Position, 1.0);

    /// 1. La click stanga pe fereasta, optiunilor: "Fragment" si "Fragment+Mediere_Normale", am modificat coordonata a 4-a din vec4 din 0.f in 1.f, 
    /// astfel incat iluminarea sa nu mai fie aplicata asupra cubului.  
    Normal = vec3(projection * view * vec4(in_Normal, 1.f));
    ///

    /// 4. La click stanga pe fereasta, optiunilor: "Fragment" si "Fragment+Mediere_Normale", am modificat coordonata a 4-a din vec4 din 1.f in 0.f, 
    /// astfel incat iluminarea sa aplicata exact la fel indiferent de miscarea observatorului in scena.
    inLightPos = vec3(projection * view * vec4(lightPos, 0.f));
    ///

    inViewPos = vec3(projection * view * vec4(viewPos, 1.0f));
    FragPos = vec3(gl_Position);
} 
