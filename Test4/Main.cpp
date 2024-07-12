# include <Siv3D.hpp> // Siv3D v0.6.15

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	const Font font{FontMethod::MSDF, 48, Typeface::Medium};

	while (System::Update())
	{
	}
}
