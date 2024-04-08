#include "debugger.h"

using namespace AngelScript;

class Script2 : public Script
{
public:
	using Script::Script;

	ScriptModule& GetModule() const { return *_getModule(); }
	asIScriptContext* GetContext() const { return GetModule().context; }
	asIScriptFunction* GetFunction(StringView name) const { return _getFunction(name); }
};

void Main2()
{
	Script2 script(U"debug_test/a.as");
	script.reload(ScriptCompileOption::BuildWithLineCues);

	const auto fn = script.getFunction<void()>(U"Test");

	CDebugger dbg;

	const auto ctx = script.GetContext();
	if (ctx->GetEngine()->GetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES) == 1)
	{
		std::cout <<
			"WARNING: Engine property asEP_BUILD_WITHOUT_LINE_CUES is set! This will cause strange behavior while debugging.";
	}

	dbg.SetEngine(Script::GetEngine());

	ctx->ClearLineCallback();
	ctx->SetLineCallback(asMETHOD(CDebugger, LineCallback), &dbg, asCALL_THISCALL);
	// dbg.AddFileBreakPoint("debug_test/a.as", 9);
	// dbg.TakeCommands(ctx);

	String exception;
	fn.tryCall(exception);

	while (System::Update())
	{
	}
}
