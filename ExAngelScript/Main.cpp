#include <iso646.h>
# include <Siv3D.hpp> // Siv3D v0.6.14

namespace
{
	void breakpoint(uint32 value)
	{
		Print(U"Called {}"_fmt(value));
	}
}

void reloadScript(Script& script)
{
	ClearPrint();

	if (not script.reload())
	{
		Print(script.getMessages());
		return;
	}
	const auto scriptFunc = script.getFunction<void()>(U"setup");
	String exception;
	scriptFunc.tryCall(exception);
	if (exception.isEmpty() == false)
	{
		Print(exception);
	}
	else
	{
		Print(U"succeeded");
	}
}

void Main()
{
	Scene::SetBackground(ColorF{0.2});

	Script script{U"script/my_script.as"};
	script.GetEngine()->RegisterGlobalFunction(
		"void breakpoint(uint32)",
		AngelScript::asFunctionPtr(breakpoint),
		AngelScript::asCALL_CDECL);

	reloadScript(script);

	const DirectoryWatcher directoryWatcher{U"script"};

	while (System::Update())
	{
		if (directoryWatcher.retrieveChanges().includes_if([](const auto& c)
		{
			return FileSystem::FileName(c.path) == U"my_script.as";
		}))
		{
			reloadScript(script);
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
