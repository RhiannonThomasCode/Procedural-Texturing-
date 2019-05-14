// cRenderClass.cpp
// 
//////////////////////////////////////////////////////////////////////////////////////////
// includes 
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Declarations 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass graphics( SCREEN_WIDTH, SCREEN_HEIGHT );

unsigned int	m_tex[256];

float angle;
//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - constructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::cRenderClass(const int w, const int h)
{
	m_sw = w;
	m_sh = h;

	m_OPENGL_2_0_SUPPORTED = true;

	// generate a number of textures...
	glGenTextures(2, m_tex);
}

//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - destructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::~cRenderClass()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// loop() - enters game loop
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::loop()
{
	glutMainLoop();
}

extern cShaderInfo	ShaderInfo;

//////////////////////////////////////////////////////////////////////////////////////////
// initShaders() - init GLSL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::initShaders()
{
	GLenum err = glewInit();

	if( GLEW_OK != err )
	{
		printf("Error: Glew is not supported\n\n");
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		printf("OpenGL 2.0 is supported\n\n");
	}
	else
	{
		printf("Error: OpenGL 2.0 is not supported\n\n");
		m_OPENGL_2_0_SUPPORTED = false;
	}

	ShaderInfo.create();
}

//////////////////////////////////////////////////////////////////////////////////////////
// initialize glut stuff
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::create(int argc, _TCHAR* argv[])
{
	// initialise the glut library
	glutInit(&argc, argv);

	// set up the initial display mode
	// need both double buffering and z-buffering
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// set the initial window position
	glutInitWindowPosition(100, 100);

	// set the initial window size
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// create and name the window
	glutCreateWindow("Using GLSL Shaders for Post Processing (Multi-Pass)!");

	// reshape callback for current window
	glutReshapeFunc(setViewport3D);

	// set display callback for current window
	glutDisplayFunc(renderScene);	

	// set up the global idle callback
	glutIdleFunc(update);

	initShaders();	
}

//////////////////////////////////////////////////////////////////////////////////////////
// setViewport2D() - sets up the viewport ready for a screen grab
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::setViewport2D()
{
	glLoadIdentity();
		
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, m_sw, 0, m_sh);
	glMatrixMode(GL_MODELVIEW);

	// set up the viewport
	glViewport
	(
		0,							// lower left x position
		0,							// lower left y position
		m_sw,	// viewport width
		m_sh	// viewport height
	);
}

//////////////////////////////////////////////////////////////////////////////////////////
// render() - 
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::render(int pass)
{
	if (pass == 0)
	{
		///////////////////////////////////////////////////////
		// Render the 3D object during the first PASS
		///////////////////////////////////////////////////////

		// set the viewport to 3D - so you can render your 3D objects on the first pass
		setViewport3D(SCREEN_WIDTH, SCREEN_HEIGHT);

		// disable shader program
		glUseProgram(0);

		// clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/////////////////////////////////////////////////////////////
		// draw object here using OpenGL standard calls
		/////////////////////////////////////////////////////////////
		glPushMatrix();

		glLoadIdentity();

		glTranslatef(0.0, 0.0, -2.0);
		glRotatef(angle, 1.0f, 1.0f, 0.0f);
		angle += 0.5;


		glEnable(GL_DEPTH_TEST);

		// select the shader we wish to use here..
		cShader *pList = ShaderInfo.getList();
		int shdr = 0;

		if (shdr >= ShaderInfo.shaderCount())
		{
			printf("Error: Can't access requested shader \n");
			shdr = 0;
		}

		// enable shader program..
		glUseProgram(pList[shdr].program());

		glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);

		// render your object here..
		glutSolidTorus(0.25, 0.5, 50, 50);

	

		glPopMatrix();
		
		glFlush();

		

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////	
		if (PASSES > 1)
			return;
	}
	else
		if (pass == 1)
		{
			/////////////////////////////////////////////////////
			/* Grab rendered image from back-buffer and convert it to a texture during the second PASS

			 During this pass, we perform all necessary intensity calculations and write the contents into the texture
			 Steps required:
					Divide texture space up into equal sized squares. On the next PASS each square will be replaced with a white circle
					The size of the white circle is calculated on this PASS. This is based on the averaged intensity values for all
					pixels contained within the grid
					Store the average intensity value in the lower right pixel of the square. You will be able to read in and use this value
					during the next PASS*/

					/////////////////////////////////////////////////////

			glDisable(GL_DEPTH_TEST);

			//flatten the viewport - i.e. convert to image space so that we can perform the post-production visual effect
			setViewport2D();

			glEnable(GL_TEXTURE_2D);

			//grab image from back buffer - and convert it to a texture
			glReadBuffer(GL_BACK);
			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_sw, m_sh, 0);

			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glDrawBuffer(GL_BACK);

			// render selected texture map here..
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			// select the shader we wish to use here..
			cShader *pList = ShaderInfo.getList();
			int shdr = 2;

			if (shdr >= ShaderInfo.shaderCount())
			{
				printf("Error: Can't access requested shader\n");
				shdr = 0;
			}

			//enable shader program..
			glUseProgram(pList[shdr].program());

			glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);


			/* draw full screen textured quad here...
			 the texture is the one we just grabbed from the back buffer, i.e. a screen grab of the first PASS render*/
	//		glBegin(GL_QUADS);

	////////		 render the final image here..
	//		glColor3f(1.0, 1.0, 1.0);

	//		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	//		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, m_sh);
	//		glTexCoord2f(1.0f, 1.0f); glVertex2f(m_sw, m_sh);
	//		glTexCoord2f(1.0f, 0.0f); glVertex2f(m_sw, 0);

	//		glEnd();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glDisable(GL_TEXTURE_2D);
			glUseProgram(0);

			if (PASSES > 2)
				return;
		}
		//glutSwapBuffers();

		else
		{
//			/////////////////////////////////////////////////////
//		/*	 Grab rendered image again from back-fuffer and convert it to a texture during the second PASS
//			
//			 During this pass, we read in the lower right pixel intensity value and use this to scale the radius of the circle we wish to draw
//				STEPS:
//					1) determine the centre of the square
//					2) calculate the maximum size for the circle, i.e. max_radius = sizeof_square/2.0
//					3) calculate the distance from this centre point to the pixel currently being rendered
//					4) Determoine if the pixel is contained within the circle we wish to draw
//							If the distance is less than the intensity value multiplied by the radius, then we render the pixel white
//							otherwise this pixel is outside the circle and gets rendered black*/
//					
//			/////////////////////////////////////////////////////
//
//			// flatten the viewport - i.e. convert to image space
			setViewport2D();

			glEnable(GL_TEXTURE_2D);

			 //grab the contents of the back buffer again and convert this into a texture..
			// NB> This iamge is the same image as last time, with the addition of averaged pixel intensity values stored
			 //in the lower right pixel of each square

			glReadBuffer(GL_BACK);
			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_sw, m_sh, 0);

			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glDrawBuffer(GL_BACK);

			// render selected texture map here..
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


			// select the shader that will draw the white circles to screen
			cShader *pList = ShaderInfo.getList();
			int shdr = 1;

			if (shdr >= ShaderInfo.shaderCount())
			{
				printf("Error: Can't access requested shader\n");
				shdr = 0;
			}

			 //enable shader program..
			glUseProgram(pList[shdr].program());

			glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);
			
			 //full-screen quad
			//glBegin(GL_QUADS);


			//// render the final image here..
			//glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
			//glTexCoord2f(0.0f, 1.0f); glVertex2f(0, m_sh);
			//glTexCoord2f(1.0f, 1.0f); glVertex2f(m_sw, m_sh);
			//glTexCoord2f(1.0f, 0.0f); glVertex2f(m_sw, 0);

			//glEnd();
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_tex[0]);
			glDisable(GL_TEXTURE_2D);
			glUseProgram(0);
		}

	 //finally swap the buffers of the current window and display the image to screen
	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// setViewport3D() - gets called initially and whenever the window get resized
// resizing has been locked
//////////////////////////////////////////////////////////////////////////////////////////
void setViewport3D(GLint w, GLint h)
{
	if (h == 0 || w == 0)
		return;

	//Set a new projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55.0, (GLdouble)w / (GLdouble)h, 0.5, 100.0);


	GLfloat lightpos0[] = { 0.0f, 0.5f, 0.85f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);

	GLfloat lightpos1[] = { 0.707f, -0.707f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport
	(
		0,				// lower left x position
		0,				// lower left y position
		(GLsizei)SCREEN_WIDTH,	// viewport width
		(GLsizei)SCREEN_HEIGHT	// viewport height
	);
}
