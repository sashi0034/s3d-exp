# include <Siv3D.hpp> // Siv3D v0.6.12
# include <windows.h>

void ConsoleShow()
{
	// アタッチするコンソールが存在しない場合は、新しいコンソールを作成
	// if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		AllocConsole();
	}

	// 標準出力とエラー出力をコンソールにリダイレクト
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	// ログの出力
	std::cout << "Hello, Console!" << std::endl;
	std::cerr << "This is an error message." << std::endl;

	const Texture emoji{U"🛰️"_emoji};

	while (System::Update())
	{
		emoji.drawAt(Scene::Center());
	}

	FreeConsole();
}
