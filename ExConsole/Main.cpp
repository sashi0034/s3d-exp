# include <Siv3D.hpp> // Siv3D v0.6.12

void Main()
{
	ChildProcess childLogger{U"../../ExConsole2/bin/Release/net7.0-windows/ExConsole2.exe", Pipe::StdInOut};

	double t{};

	Print(U"Started 🚀");
	while (System::Update())
	{
		t += Scene::DeltaTime();
		if (t > 0.5)
		{
			t = 0;
			childLogger.ostream() << "Hello C# WPF from C++ Siv3D" << std::endl;
		}
	}

	childLogger.terminate();
}
