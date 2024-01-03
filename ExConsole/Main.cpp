# include <Siv3D.hpp> // Siv3D v0.6.12
# include <windows.h>

void Main()
{
	ChildProcess childLogger{U"../../ExConsole2/bin/Debug/net7.0-windows/ExConsole2.exe"};

	const Texture emoji{U"🛰️"_emoji};

	while (System::Update())
	{
		emoji.drawAt(Scene::Center());
	}

	childLogger.terminate();

	FreeConsole();
}
