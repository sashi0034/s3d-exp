# include <Siv3D.hpp> // Siv3D v0.6.12
# include <windows.h>

void Main()
{
	ChildProcess childLogger{U"../../ExConsole2/bin/Debug/net7.0-windows/ExConsole2.exe", Pipe::StdInOut};

	const Texture emoji{U"🛰️"_emoji};

	double t{};

	while (System::Update())
	{
		t += Scene::DeltaTime();
		if (t > 0.5)
		{
			t = 0;
			childLogger.ostream() << "Hello C# WPF from C++ Siv3D";
		}

		// 🛰️
		emoji.drawAt(Scene::Center());
	}

	childLogger.terminate();

	FreeConsole();
}
