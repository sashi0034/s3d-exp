#include <iso646.h>

void callStart(Script main)
{
	auto start = main.getFunction<void()>(U"start");
	String exception;
	start.tryCall(exception);
	if (not exception.empty()) Console.writeln(exception);
}

void printAngelInfo(const AngelScript::asIScriptEngine& engine)
{
	for (int i = 0; i < engine.GetObjectTypeCount(); i++)
	{
		const auto t = engine.GetObjectTypeByIndex(i);
		if (not t) continue;
		std::cout << std::format("type {}::{}\n", t->GetNamespace(), t->GetName());
		for (int j = 0; j < t->GetMethodCount(); ++j)
		{
			const auto m = t->GetMethodByIndex(j);
			std::cout << std::format("\ttype_method {}\n", m->GetDeclaration(false, true, false));
			std::cout << std::format("\ttype_method {}\n", m->GetDeclaration(false, true, false));
		}
		for (int j = 0; j < t->GetPropertyCount(); ++j)
		{
			std::cout << std::format("\ttype_property {}\n", t->GetPropertyDeclaration(j, true));
		}
	}

	for (int i = 0; i < engine.GetGlobalFunctionCount(); i++)
	{
		const auto f = engine.GetGlobalFunctionByIndex(i);
		if (not f) continue;
		std::cout << std::format("function {}\n", f->GetDeclaration(true, true, false));
	}

	for (int i = 0; i < engine.GetGlobalPropertyCount(); i++)
	{
		const char* name;
		const char* ns;
		int type;
		engine.GetGlobalPropertyByIndex(i, &name, &ns, &type, nullptr, nullptr, nullptr, nullptr);
		const auto t = engine.GetTypeInfoById(type);
		if (not t) continue;
		std::cout << std::format("property {}::{} {}::{}\n", t->GetNamespace(), t->GetName(), ns, name);
	}

	for (int i = 0; i < engine.GetEnumCount(); i++)
	{
		const auto e = engine.GetEnumByIndex(i);
		if (not e) continue;
		std::cout << std::format("enum {}::{}\n", e->GetNamespace(), e->GetName());
		for (int j = 0; j < e->GetEnumValueCount(); ++j)
		{
			std::cout << std::format(
				"\tenum_value {}\n", e->GetEnumValueByIndex(j, nullptr));
			// std::cout << std::format(
			// 	"enum_value {}::{}::{}\n", e->GetNamespace(), e->GetName(), e->GetEnumValueByIndex(j, nullptr));
		}
	}
}

void Main()
{
	Console.open();
	printAngelInfo(*Script::GetEngine());

	while (System::Update())
	{
	}

	Script main(U"script.as");

	// F5を押したらスクリプト側のSystem::Updateはfalseが返る

	Console.writeln(main.getMessages());

	callStart(main);
	DirectoryWatcher directoryWatcher(U"./");

	while (System::Update())
	{
		// TimeProfiler profiler(U"Main");

		if (directoryWatcher.retrieveChanges().size() > 0)
		{
			Console.writeln(U"\nRELOAD ============================");
			if (main.reload())
			{
				Console.writeln(U"OK");
				callStart(main);
			}
			else
			{
				Console.writeln(main.getMessages());
			}
			directoryWatcher.clearChanges();
		}
		auto mainFunc = main.getFunction<void()>(U"Main");
		String exception;
		mainFunc.tryCall(exception);
		if (not exception.empty()) Console.writeln(exception);
	}
}
