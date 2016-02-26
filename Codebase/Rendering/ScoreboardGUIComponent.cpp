#define STB_TRUETYPE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "ScoreboardGUIComponent.h"
#include "Helpers\stb_truetype.h"

unsigned char ttf_buffer[1 << 20];
unsigned char temp_bitmap[512 * 512];
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;


ScoreboardGUIComponent::ScoreboardGUIComponent(Material* material, Texture* texture, float z, const std::string& name) : GUIComponent(material, texture, z),
	m_ScoreA(0),
	m_ScoreB(0),
	m_TimeRem(300)
{

	m_Name = name;
	m_Material = material;
	m_Texture = texture;
	depth = z;



	material->Set("diffuseTex", m_Texture);

	Vec3Graphics* points = new Vec3Graphics[4];
	Vec3Graphics start(0.25f, -0.6, 0);
	Vec3Graphics end(-0.25f, -1.0f, 0);
	points[0] = Vec3Graphics(start.x, start.y, start.z);
	points[1] = Vec3Graphics(start.x, end.y, start.z);
	points[2] = Vec3Graphics(end.x, end.y, end.z);
	points[3] = Vec3Graphics(end.x, start.y, end.z);

	float texLength = (start - end).Length();
	m_Mesh = Mesh::GenerateQuad(points, Vec2Graphics(texLength, texLength));
	this->SetRenderComponent(new RenderComponent(m_Material, m_Mesh));

}

void ScoreboardGUIComponent::Update()
{
	Update(0, 0, 50);
}

void ScoreboardGUIComponent::Update(int scoreA, int scoreB, int timeRem)
{
	m_ScoreA = scoreA;
	m_ScoreB = scoreB;
	m_TimeRem = timeRem;
}

void ScoreboardGUIComponent::Render()
{
	Update();
	this->GetRenderComponent()->Draw();
}
//
//void my_stbtt_initfont(void)
//{
//	fread(ttf_buffer, 1, 1 << 20, fopen("c:/windows/fonts/times.ttf", "rb"));
//	stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
//	// can free ttf_buffer at this point
//	glGenTextures(1, &ftex);
//	glBindTexture(GL_TEXTURE_2D, ftex);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
//	// can free temp_bitmap at this point
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//}
//
//void my_stbtt_print(float x, float y, char *text)
//{
//	// assume orthographic projection with units = screen pixels, origin at top left
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, ftex);
//	glBegin(GL_QUADS);
//	while (*text) {
//		if (*text >= 32 && *text < 128) {
//			stbtt_aligned_quad q;
//			stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1); //1=opengl & d3d10+,0=d3d9
//			glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
//			glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
//			glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
//			glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
//		}
//		++text;
//	}
//	glEnd();
//}