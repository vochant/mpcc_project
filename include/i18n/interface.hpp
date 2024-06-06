#pragma once

#include "system/current.hpp"
#include "i18n/i18n.hpp"

I18N i18n;

namespace _I18N {
	bool _isI18nSupported() {
		#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
		return true;
		#else
		return false;
		#endif
	}

	void load_default_i18n(I18N* i18n) {
		i18n->Register({"lang.name", ":lang.name"});
		i18n->Register({"error.call.tooLessArgs", ":error.call.tooLessArgs [Expected] [Current]"});
		i18n->Register({"error.call.incorrectArgs", ":error.call.incorrectArgs [Expected] [Current]"});
		i18n->Register({"error.call.nativeFormat", ":error.call.nativeFormat"});
		i18n->Register({"error.call.nativeUnhandled", ":error.call.nativeUnhandled"});
		i18n->Register({"error.ast.program", ":error.ast.program"});
		i18n->Register({"error.evaluator.moreExpect", ":error.evaluator.moreExpect [Type]"});
		i18n->Register({"error.evaluator.constructorType", ":error.evaluator.constructorType"});
		i18n->Register({"error.ast.functionInner", ":error.ast.functionInner"});
		i18n->Register({"error.call.notExecutable", ":error.call.notExecutable [Subject]"});
		i18n->Register({"error.call.notExecutable1", ":error.call.notExecutable1"});
		i18n->Register({"error.evaluator.nullAssignUnexpected", ":error.evaluator.nullAssignUnexpected"});
		i18n->Register({"error.call.xcrementArgs", ":error.call.xcrementArgs"});
		i18n->Register({"error.call.nativeFormat1", ":error.call.nativeFormat1 [Subject]"});
		i18n->Register({"error.call.nativeUnhandled1", ":error.call.nativeUnhandled1 [Subject]"});
		i18n->Register({"error.call.indexArgs", ":error.call.indexArgs"});
		i18n->Register({"error.index.stringAt", ":error.index.stringAt [Begin] [End] [Index]"});
		i18n->Register({"error.index.type", ":error.index.type"});
		i18n->Register({"error.index.incorrectRange.string", ":error.index.incorrectRange.string [Start] [End]"});
		i18n->Register({"error.index.stringAt1", ":error.index.stringAt1 [Begin] [End] [I0] [I1]"});
		i18n->Register({"error.index.arrayAt", ":error.index.arrayAt [Begin] [End] [Index]"});
		i18n->Register({"error.index.incorrectRange.array", ":error.index.incorrectRange.array [Start] [End]"});
		i18n->Register({"error.index.arrayAt1", ":error.index.arrayAt1 [Begin] [End] [I0] [I1]"});
		i18n->Register({"error.index.typeSupport", ":error.index.typeSupport"});
		i18n->Register({"error.infix.notIdentifier", ":error.infix.notIdentifier"});
		i18n->Register({"error.infix.noSuchMember", ":error.infix.noSuchMember"});
		i18n->Register({"error.infix.memberPrivate", ":error.infix.memberPrivate"});
		i18n->Register({"error.infix.noSuchEnumerate", ":error.infix.noSuchEnumerate"});
		i18n->Register({"error.call.prefixArgs", ":error.call.prefixArgs"});
		i18n->Register({"error.stat.type", ":error.stat.type"});
		i18n->Register({"error.value.type", ":error.value.type"});
		i18n->Register({"error.statement.type", ":error.statement.type"});
		i18n->Register({"error.call.infixArgs", ":error.call.infixArgs"});
		i18n->Register({"error.call.argData", ":error.call.argData"});
		i18n->Register({"error.unhandled", ":error.unhandled [Reason]"});
		i18n->Register({"error.program.init", ":error.program.init"});
		i18n->Register({"error.unexpected_node_type", ":error.unexpected_node_type"});
		i18n->Register({"error.argsize", ":error.argsize [How]"});
		i18n->Register({"error.iconstructor", ":error.iconstructor"});
		i18n->Register({"error.datatype", ":error.datatype [How]"});
		i18n->Register({"error.datavalue", ":error.datavalue"});
		i18n->Register({"error.notfound", ":error.notfound [Subject]"});
		i18n->Register({"error.moretype", ":error.moretype [What]"});
		i18n->Register({"error.assignconstant", ":error.assignconstant [What]"});
		i18n->Register({"error.assignBetweenTypes", ":error.assignBetweenTypes [T1] [T2]"});
		i18n->Register({"error.inherit", ":error.inherit [Name]"});
		i18n->Register({"error.leveltype", ":error.leveltype"});
		i18n->Register({"error.xcrementType", ":error.xcrementType"});
		i18n->Register({"error.alreadyExists", ":error.alreadyExists [Name]"});
		i18n->Register({"error.forElement", ":error.forElement [How]"});
		i18n->Register({"error.xcrement", ":error.xcrement [How]"});
		i18n->Register({"error.outOfRange", ":error.outOfRange [What]"});
		i18n->Register({"error.invalid", ":error.invalid [What]"});
		i18n->Register({"error.memberError", ":error.memberError [How]"});
		i18n->Register({"error.infix", ":error.infix [What] [Left] [Right]"});
		i18n->Register({"error.prefix", ":error.prefix [What] [Type]"});
		i18n->Register({"error.scopeLeft", ":error.scopeLeft"});
		i18n->Register({"error.forStat", ":error.forStat"});
		i18n->Register({"error.functionNoArgs", ":error.functionNoArgs"});
		i18n->Register({"error.moreName", ":error.moreName"});
		i18n->Register({"error.argSplit", ":error.argSplit"});
	}
}

void InitI18N() {
	if (_I18N::_isI18nSupported()) {
		Module mod;
		mod.load("master.json", _i18n_name + ".language");
		if (i18n.Register(mod)) {
			_I18N::load_default_i18n(&i18n);
		}
	}
	else {
		_I18N::load_default_i18n(&i18n);
	}
}