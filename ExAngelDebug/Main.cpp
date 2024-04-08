# include <Siv3D.hpp>

#include "AScript.hpp"

void Main()
{
	String error;

	const AScript script{U"./debug_test//a.as"};
	for (const auto& message : script.getMessages()) std::cout << message << std::endl;

	const auto fullPath = FileSystem::FullPath(script.path()).narrow();
	AScript_detail::s_debugger.AddBreakPoint({.line = 6, .sectionName = fullPath});

	script.getFunction<void()>(U"Test").tryCall(error);

	while (System::Update())
	{
	}
}
