#include "menu.h"
#include "constants.h"
#include "GL/GL.h"
#include "GL/GLU.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"

#include "esp.h"
#include "settings.h"

const ImVec2 initWindowSize = ImVec2(500, 700);
bool showMenu = false;
bool initialized = false;
bool contextCreated = false;
HWND gameWindow;
HGLRC myContext;
HGLRC gameContext;
WNDPROC originalWindowProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK newWNDProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (showMenu) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return 0;
		switch (uMsg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return 0;
		}
	}
	return CallWindowProc(originalWindowProc, hWnd, uMsg, wParam, lParam);
}

void Menu::toggleMenu() {
	showMenu = !showMenu;
	ImGuiIO& io = ImGui::GetIO();

	io.WantCaptureMouse = showMenu;
	io.WantCaptureKeyboard = showMenu;
	io.MouseDrawCursor = showMenu;
	originalSetRelativeMouseMode( !showMenu );


}

void Menu::init()
{
	ImGui::CreateContext();
	gameWindow = FindWindowA( NULL , "AssaultCube" );
	originalWindowProc = ( WNDPROC ) SetWindowLongPtr(gameWindow, GWLP_WNDPROC, (LONG_PTR)newWNDProc);

	ImGui_ImplWin32_Init( gameWindow );
	ImGui_ImplOpenGL2_Init();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.Fonts->AddFontDefault();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui::SetNextWindowSize(initWindowSize);

	initialized = true;
	std::cout << "Menu Initialized" << std::endl;
}

void Menu::startRenderer()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}


void testinSettings() {
	if ( !ImGui::BeginTabItem( "Testing" ) ) {
		return;
	}
	ImGui::Text( "Hello, WOrld" );

	if ( ImGui::Button( "Tp up" ) ) {
		localPlayerPtr->pos.y += 5;
	}
	ImGui::EndTabItem();
}

void espSettings() {
	if ( !ImGui::BeginTabItem( "ESP" ) ) {
		return;
	}
	ImGui::Checkbox( "Enabled" , &Settings::ESP::enabled );
	ImGui::Checkbox( "Draw Team" , &Settings::ESP::drawTeam );
	ImGui::ColorEdit4( "Team Color" , ( float* ) &Settings::ESP::teamColor->Value );
	ImGui::ColorEdit4( "Enemy Color" , ( float* ) &Settings::ESP::enemyColor->Value );
	ImGui::EndTabItem();
}

void aimbotSettings() {
	if ( !ImGui::BeginTabItem( "Aimbot" ) ) {
		return;
	}
	ImGui::Checkbox( "Enabled" , &Settings::Aimbot::enabled );
	ImGui::Checkbox( "Smoothing" , &Settings::Aimbot::smoothing );
	ImGui::SliderFloat( "Smoothing Amount" , &Settings::Aimbot::smoothingAmount, 0.0f, 10.0f );
	ImGui::Checkbox( "Check in Fov" , &Settings::Aimbot::checkInFov );
	ImGui::SliderFloat( "Fov" , &Settings::Aimbot::fov, 0.0f, 180.0f );
	ImGui::Checkbox( "Draw Fov Circle" , &Settings::Aimbot::drawFovCircle );
	ImGui::EndTabItem();
}

void Menu::render()
{
	if ( !showMenu ) return;

	ImGui::Begin("Menu", &showMenu);
	ImGui::Text("Hello,  world!");
	if ( ImGui::BeginTabBar("Tabbar") ) {
		espSettings();
		aimbotSettings();
		testinSettings();
		ImGui::EndTabBar();
	}


	originalSetRelativeMouseMode( !showMenu );
	ImGui::End();

}

void setupContext(HDC& hdc) {
	myContext = wglCreateContext(hdc);

	wglMakeCurrent( hdc , myContext );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[ 4 ];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glOrtho( 0 , viewport[ 2 ] , viewport[ 3 ] , 0 , -1 , 1 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	contextCreated = true;
}

void Menu::endRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

BOOL __stdcall Menu::newSwapBuffers( HDC hdc )
{
	if ( !initialized ) {
		Menu::init();
		return originalSwapBuffers( hdc );
	}

	gameContext = wglGetCurrentContext();
	if ( !contextCreated ) {
		setupContext(hdc);
	}
	wglMakeCurrent( hdc , myContext );

	Menu::startRenderer();
	Menu::render();
	ESP::drawEsp();
	ESP::aimbot();
	Menu::endRender();

	wglMakeCurrent( hdc , gameContext );
	return originalSwapBuffers(hdc);

}
