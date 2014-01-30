#ifndef GL_SHADER_FONTSTASH_H
#define GL_SHADER_FONTSTASH_H

struct FONScontext* gl_shader_fonsCreate(int width, int height, int flags);
void gl_shader_fonsDelete(struct FONScontext* ctx);

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif

#ifdef GL_SHADER_FONTSTASH_IMPLEMENTATION

struct GLFONScontext {
    // Program handle
    GLuint program;

    // Locations
    GLint position_location;
    GLint tex_coord_location;
    GLint tex_location;

    // Required to have seperate buffers for verticies and tex coords
    GLuint vbo_vert, vbo_tex;

    GLuint tex;
    int width, height;
};

static void glfons__create_shaders(void * userPtr)
{
    struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;

    const GLchar* vertexSource =
	"#version 150 core\n"
	"in vec2 position;"
	"in vec2 tex_coord;"
	"out vec2 frag_tex_coord;"
	"void main() {"
	"   frag_tex_coord = tex_coord;"
	"   gl_Position = vec4(position.x/800, position.y/-800, 0.0, 1.0);"
	"}";
    const GLchar* fragmentSource =
	"#version 150 core\n"
	"in vec2 frag_tex_coord;"
	"out vec4 outColor;"
	"uniform sampler2D tex;"
	"void main() {"
	"   float alpha = texture(tex, frag_tex_coord).r;"
	"   outColor = vec4(1.0, 1.0, 1.0, alpha);"
	"}";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Compile frag shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    gl->program = glCreateProgram();
    glAttachShader(gl->program, vertexShader);
    glAttachShader(gl->program, fragmentShader);
   
    // Link and use program
    glLinkProgram(gl->program);
//    glUseProgram(gl->program);

    // Get position location
    gl->position_location = glGetAttribLocation(gl->program, "position");
    // Get tex_coord location
    gl->tex_coord_location = glGetAttribLocation(gl->program, "tex_coord");
    // Get tex uniform location
    gl->tex_location = glGetUniformLocation(gl->program, "tex");
}

static void glfons__create_buffers(void * userPtr)
{
    struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;

    // Set vertex array object
    // Must use VAO with VBO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &gl->vbo_vert);
    glGenBuffers(1, &gl->vbo_tex);
}

static int glfons__renderCreate(void* userPtr, int width, int height)
{
	struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;
	glGenTextures(1, &gl->tex);
	if (!gl->tex) return 0;
	gl->width = width;
	gl->height = width;
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gl->width, gl->height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Programable pipeline requires explicit buffer and shader managmement
        glfons__create_buffers(gl);
        glfons__create_shaders(gl);

	return 1;
}

static void glfons__renderUpdate(void* userPtr, int* rect, const unsigned char* data)
{
	struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;
	int w = rect[2] - rect[0];
	int h = rect[3] - rect[1];

	if (gl->tex == 0) return;
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, gl->width);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect[0]);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, rect[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], w, h, GL_RED,GL_UNSIGNED_BYTE, data);
}

static void glfons__renderDraw(void* userPtr, const float* verts, const float* tcoords, const unsigned int* colors, int nverts)
{
	struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;
	if (gl->tex == 0) return;

        // Size of each vertex in bytes
        size_t vert_size = 2*sizeof(GL_FLOAT);

        // Draw texture
	// TO DO - add color
        glUseProgram(gl->program);

        // Bind vert buffer
        glBindBuffer(GL_ARRAY_BUFFER, gl->vbo_vert);
        // Fill vert buffer
        glBufferData(GL_ARRAY_BUFFER, nverts*vert_size, verts, GL_DYNAMIC_DRAW);
	// Get and enable vertex pointer
        glVertexAttribPointer(gl->position_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(gl->position_location);

        // Bind tex coord buffer
        glBindBuffer(GL_ARRAY_BUFFER, gl->vbo_tex);
        // Fill tex coord buffer
        glBufferData(GL_ARRAY_BUFFER, nverts*vert_size, tcoords, GL_DYNAMIC_DRAW);
	// Get and enable tex coord pointer
        glVertexAttribPointer(gl->tex_coord_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(gl->tex_coord_location);

        glBindTexture(GL_TEXTURE_2D, gl->tex);
        glUniform1i(gl->tex_location, 0);
        glDrawArrays(GL_TRIANGLES, 0, nverts);
}

static void glfons__renderDelete(void* userPtr)
{
	struct GLFONScontext* gl = (struct GLFONScontext*)userPtr;
	if (gl->tex)
		glDeleteTextures(1, &gl->tex);
	gl->tex = 0;
	free(gl);

        // Need to cleanup here....
}

struct FONScontext* gl_shader_fonsCreate(int width, int height, int flags)
{
	struct FONSparams params;
	struct GLFONScontext* gl;

	gl = (struct GLFONScontext*)malloc(sizeof(struct GLFONScontext));
	if (gl == NULL) goto error;
	memset(gl, 0, sizeof(struct GLFONScontext));

	memset(&params, 0, sizeof(params));
	params.width = width;
	params.height = height;
	params.flags = flags;
	params.renderCreate = glfons__renderCreate;
	params.renderUpdate = glfons__renderUpdate;
	params.renderDraw = glfons__renderDraw; 
	params.renderDelete = glfons__renderDelete;
	params.userPtr = gl;

	return fonsCreateInternal(&params);

error:
	if (gl != NULL) free(gl);
	return NULL;
}

void gl_shader_fonsDelete(struct FONScontext* ctx)
{
	fonsDeleteInternal(ctx);
}

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

#endif