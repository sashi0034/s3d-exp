# include <Siv3D.hpp> // Siv3D v0.6.12
#include "Windows.h"

namespace
{
	LONG_PTR g_baseProc = 0;

	LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCPAINT)
		{
			HDC hdc;
			hdc = GetWindowDC(hWnd);

			// ここで非クライアント領域の描画をカスタマイズ
			// 例: ダークカラーで塗りつぶし
			RECT rect;
			GetWindowRect(hWnd, &rect);
			SetBkColor(hdc, RGB(45, 45, 48)); // ダークグレー
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

			ReleaseDC(hWnd, hdc);
			return 0;
		}

		return CallWindowProc(reinterpret_cast<WNDPROC>(g_baseProc), hWnd, message, wParam, lParam);
	}
}

void Main()
{
	// 自前ウィンドウ処理
	const auto hWnd = static_cast<HWND>(s3d::Platform::Windows::Window::GetHWND());
	g_baseProc = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(::CustomWindowProc));

	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	Window::SetStyle(WindowStyle::Sizable);

	const Texture emoji{U"🥦"_emoji};

	while (System::Update())
	{
		emoji.drawAt(Scene::Center());
	}
}
