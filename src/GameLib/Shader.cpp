#include "Shader.h"

void Shader::SimpleShader(cpu_ps_io& io)
{
	io.color = io.p.color;
}