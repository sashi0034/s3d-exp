#include <iso646.h>
# include <Siv3D.hpp> // Siv3D v0.6.14

namespace
{
	using namespace AngelScript;

	class ScriptFunctionWrapper
	{
	public:
		ScriptFunctionWrapper() = default;
		explicit ScriptFunctionWrapper(asIScriptFunction* ptr): m_ptr(ptr) { if (ptr) ptr->AddRef(); }
		~ScriptFunctionWrapper() { if (m_ptr) m_ptr->Release(); }
		// asIScriptFunction* operator->() const { return m_ptr; }
		asIScriptFunction* operator*() const { return m_ptr; }
		operator bool() const { return m_ptr; }

	private:
		asIScriptFunction* m_ptr{};
	};

	std::array<ScriptFunctionWrapper, 8> g_funcs{};

	void __cdecl breakpoint(uint32 id, asIScriptFunction* func)
	{
		Print(U"Called {}"_fmt(id));

		if (0 <= id && id < g_funcs.size())
		{
			g_funcs[id] = ScriptFunctionWrapper(func);
		}
	}

	void __cdecl fire(uint32 value)
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
		return;
	}
	else
	{
		Print(U"succeeded");
	}

	for (int i = 0; i < g_funcs.size(); ++i)
	{
		if (not g_funcs[i]) continue;
		const auto ctx = script.GetEngine()->CreateContext();
		ctx->Prepare(*g_funcs[i]);
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
