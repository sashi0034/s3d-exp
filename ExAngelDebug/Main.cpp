# include <Siv3D.hpp>

#include "AScript.hpp"

void Main()
{
	String error;

	const AScript script{U"./debug_test//a.as"};
	const auto fullPath = FileSystem::FullPath(script.path()).narrow();
	AScript_detail::s_debugger.AddBreakPoint({.line = 5, .sectionName = fullPath});

	script.getFunction<void()>(U"Test").tryCall(error);

	while (System::Update())
	{
	}
}
