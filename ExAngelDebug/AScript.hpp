#pragma once

namespace AScript_detail
{
	class AScriptDebugger;
	using namespace AngelScript;

	constexpr int breakPointLineCapacity = 8192;
}

class AScript_detail::AScriptDebugger
{
public:
	void LineCallback(asIScriptContext* context)
	{
		const auto lineInfo = LineInfo::Get(context);

		const auto found = findHitBreakPoint(lineInfo);
		if (not found) return;

		const auto& breakPoint = found;
		std::cout << "BreakPoint: " << breakPoint->sectionName << " " << breakPoint->line << std::endl;
	}

	struct BreakPoint
	{
		int line;
		std::string sectionName;
	};

	void AddBreakPoint(const BreakPoint& breakPoint)
	{
		m_breakPoints[breakPoint.line].push_back(breakPoint);
	}

private:
	std::array<std::vector<BreakPoint>, breakPointLineCapacity> m_breakPoints{};

	struct LineInfo
	{
		int32 line;
		int32 column;
		std::string_view sectionName;

		static LineInfo Get(asIScriptContext* context)
		{
			LineInfo info;
			const char* sectionName;
			info.line = context->GetLineNumber(0, &info.column, &sectionName);
			info.sectionName = sectionName;
			return info;
		}
	};

	BreakPoint* findHitBreakPoint(const LineInfo& info)
	{
		for (auto& p : m_breakPoints[info.line])
		{
			if (p.sectionName == info.sectionName) return &p;
		}
		return nullptr;
	}
};

namespace AScript_detail
{
	inline AScriptDebugger s_debugger;

	inline void lineCallback(asIScriptContext* context)
	{
		s_debugger.LineCallback(context);
	}

	inline bool execute(const std::shared_ptr<ScriptModule>& module, asIScriptContext* context)
	{
		if (module->withLineCues)
		{
			module->context->SetLineCallback(asFUNCTION(lineCallback), context, asCALL_CDECL);
		}

		uint64 scriptID = module->scriptID;
		uint64 scriptStepCounter = 0;
		module->context->SetUserData(&scriptID, FromEnum(detail::ScriptUserDataIndex::ScriptID));
		module->context->SetUserData(&scriptStepCounter, FromEnum(detail::ScriptUserDataIndex::StepCounter));

		const int32 r = module->context->Execute();

		if (r == asEXECUTION_EXCEPTION)
		{
			Logger << (U"[script exception]" + Unicode::Widen(module->context->GetExceptionString()));
			return false;
		}
		else if (r == asEXECUTION_SUSPENDED)
		{
			System::Exit();
		}

		return true;
	}

	inline Optional<String> tryExecute(const std::shared_ptr<ScriptModule>& module, asIScriptContext* context)
	{
		if (module->withLineCues)
		{
			module->context->SetLineCallback(asFUNCTION(lineCallback), context, asCALL_CDECL);
		}

		uint64 scriptID = module->scriptID;
		uint64 scriptStepCounter = 0;
		module->context->SetUserData(&scriptID, FromEnum(detail::ScriptUserDataIndex::ScriptID));
		module->context->SetUserData(&scriptStepCounter, FromEnum(detail::ScriptUserDataIndex::StepCounter));

		const int32 r = module->context->Execute();

		if (r == asEXECUTION_EXCEPTION)
		{
			return Unicode::Widen(module->context->GetExceptionString());
		}
		else if (r == asEXECUTION_SUSPENDED)
		{
			System::Exit();
		}

		return none;
	}
}

template <class Type>
class AScriptFunction;

template <class Ret, class... Args>
class AScriptFunction<Ret(Args...)>
{
public:
	using return_type = Ret;

	static constexpr size_t nargs = (sizeof...(Args));

	template <size_t i>
	struct Arg
	{
		using type = typename std::tuple_element_t<i, std::tuple<Args...>>;
	};

	[[nodiscard]]
	AScriptFunction() = default;

	[[nodiscard]]
	AScriptFunction(const std::shared_ptr<ScriptModule>& module, AngelScript::asIScriptFunction* function)
		: m_module{module}, m_function{function}
	{
	}

	/// @brief スクリプト関数が空でないかを返します。
	/// @return スクリプト関数が空でない場合 true, それ以外の場合は false
	[[nodiscard]]
	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_function);
	}

	/// @brief スクリプト関数を呼び出します。
	/// @param ...args 引数
	/// @return スクリプト関数の戻り値
	Ret operator ()(Args... args) const
	{
		if (not m_function)
		{
			return Ret{};
		}

		m_module->context->Prepare(m_function);

		setArgs(0, std::forward<Args>(args)...);

		if (not execute())
		{
			return Ret{};
		}

		return getReturn<Ret>();
	}

	/// @brief スクリプト関数を呼び出します。
	/// @param ...args 引数
	/// @param exception スクリプト例外が発生した場合の例外の内容の格納先への参照
	/// @return スクリプト関数の戻り値
	Ret tryCall(Args... args, String& exception) const
	{
		if (not m_function)
		{
			return Ret();
		}

		m_module->context->Prepare(m_function);

		setArgs(0, std::forward<Args>(args)...);

		if (const auto ex = tryExecute())
		{
			exception = ex.value();

			return Ret();
		}
		else
		{
			exception.clear();
		}

		return getReturn<Ret>();
	}

	/// @brief 関数宣言を返します。
	/// @param includeParamNames パラメータ名を含む場合 `IncludeParamNames::Yes`, それ以外の場合は `IncludeParamNames::No`
	/// @return 関数宣言。失敗した場合は空の文字列
	[[nodiscard]]
	String getDeclaration(IncludeParamNames includeParamNames = IncludeParamNames::Yes) const
	{
		return detail::GetDeclaration(m_function, includeParamNames.getBool());
	}

private:
	std::shared_ptr<ScriptModule> m_module;

	AngelScript::asIScriptFunction* m_function = nullptr;

	template <class Type, class... Args2>
	void setArgs(uint32 argIndex, Type&& value, Args2&&... args) const
	{
		setArg(argIndex++, std::forward<Type>(value));

		setArgs(argIndex, std::forward<Args2>(args)...);
	}

	template <class Type>
	void setArgs(uint32 argIndex, Type&& value) const
	{
		setArg(argIndex++, std::forward<Type>(value));
	}

	void setArgs(uint32) const
	{
	}

	template <class Type>
	void setArg(uint32 argIndex, Type&& value) const
	{
		detail::SetArg<Type>(m_module, argIndex, std::forward<Type>(value));
	}

	[[nodiscard]]
	bool execute() const
	{
		return AScript_detail::execute(m_module, m_module->context);
	}

	[[nodiscard]]
	Optional<String> tryExecute() const
	{
		return AScript_detail::tryExecute(m_module, m_module->context);
	}

	template <class Type>
	[[nodiscard]]
	Type getReturn() const
	{
		return detail::GetReturnValue<Type>(m_module);
	}
};

class AScript : public Script
{
public:
	/// @brief スクリプトのコンパイルオプション
	static inline ScriptCompileOption CompileOption = ScriptCompileOption::BuildWithLineCues;

	AScript() = default;
	AScript(const FilePath& path): Script{path, CompileOption} { return; }

	/// @brief スクリプトをリロードして再コンパイルします。
	bool reload()
	{
		return Script::reload(CompileOption);
	}

	/// @brief スクリプト関数を取得します。
	/// @tparam Fty 関数の型
	/// @param decl 関数の名前または関数宣言
	/// @return スクリプト関数。取得に失敗した場合は空のスクリプト関数
	template <class Fty>
	[[nodiscard]]
	AScriptFunction<Fty> getFunction(StringView decl) const
	{
		if (isEmpty())
		{
			return {};
		}

		if (const auto func = _getFunction(decl))
		{
			return AScriptFunction<Fty>{_getModule(), func};
		}

		return {};
	}
};
