

class cRenderClass
{
public:

	cRenderClass(const int, const int);
	~cRenderClass();

	void setViewport2D();

	void create(int argc, _TCHAR* argv[]);
	void initShaders();
	void loop();

	void render(int);

	inline void OpenGL2_0(bool value){m_OPENGL_2_0_SUPPORTED = value;}
	inline bool OpenGL2_0(){return m_OPENGL_2_0_SUPPORTED;}
	
	class cShaderInfo	ShaderInfo;

private:

	int		 m_sw, m_sh;
	bool	m_OPENGL_2_0_SUPPORTED;
};