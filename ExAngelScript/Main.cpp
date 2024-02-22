#include <iso646.h>
# include <Siv3D.hpp> // Siv3D v0.6.14

namespace
{
	using namespace AngelScript;

	std::array<asIScriptFunction*, 8> funcs{};

	void __cdecl breakpoint(uint32 value, asIScriptFunction* func)
	{
		Print(U"Called {}"_fmt(value));

		funcs[0] = func;
	}

	void __cdecl fire(uint32 value)
	{
		Print(U"Called {}"_fmt(value));
	}
}

void reloadScript(Script& script)
{
	ClearPrint();
	funcs[0] = nullptr;

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
		return;
	}
	else
	{
		Print(U"succeeded");
	}

	if (funcs[0] != nullptr)
	{
		const auto ctx = script.GetEngine()->CreateContext();
		ctx->Prepare(funcs[0]);
		ctx->SetArgDWord(0, 45);
		ctx->Execute();
	}
}

void Main()
{
	Scene::SetBackground(ColorF{0.2});

	Script script{U"script/my_script.as"};
	script.GetEngine()->RegisterFuncdef("void CALLBACK(uint32)");
	script.GetEngine()->RegisterGlobalFunction(
		"void breakpoint(uint32, CALLBACK @)",
		AngelScript::asFunctionPtr(breakpoint),
		AngelScript::asCALL_CDECL);
	script.GetEngine()->RegisterGlobalFunction(
		"void fire(uint32)",
		AngelScript::asFunctionPtr(fire),
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
