#pragma once

#include "i18n/i18n.hpp"

Language get_i18i_ma_MP() {
	return Language {
		{"lang.name", "Minffosa (Minftis Dubehs)"},
		{"error.call.tooLessArgs", "Juge tos ale tos fë [Expected] srodukya, osem güma ela tos [Current] srodukya."},
		{"error.call.incorrectArgs", "Juge ale tos [Expected] srodukya, osem ela tos [Current] srodukya."},
		{"error.call.nativeFormat", "Ovale feve kadele tos letomip."},
		{"error.call.nativeUnhandled", "Ovale feve kadele tos upmip."},
		{"error.ast.program", "Otaida kadeler leto! Agsil os un el kadele idept MPCC Project i un elr lorof sil jon"},
		{"error.evaluator.moreExpect", "Evak idept etpä to srodukya id os Array pato, ag id os [Type] pato."},
		{"error.evaluator.constructorType", "Tumak sïdettole Function pato."},
		{"error.ast.functionInner", "Tumak sFunction pator Inner erdukya ScopeNode pato."},
		{"error.call.notExecutable", "Os[Subject] re sil ettole io feve ettole."},
		{"error.call.notExecutable1", "Osel re sil ettole io feve ettole."},
		{"error.evaluator.nullAssignUnexpected", "Konö etduk o Null pato."},
		{"error.call.xcrementArgs", "Ota id Increment ettole io Decrement ettole to srodukya."},
		{"error.call.nativeFormat1", "Ovale no [Subject] feve kadele tos letomip."},
		{"error.call.nativeUnhandled1", "Ovale no [Subject] feve kadele tos upmip."},
		{"error.call.indexArgs", "Tumak ag id Index ettole to re re srodukya i ota id Srodukya Etpä."},
		{"error.index.stringAt", "Juge ale String pator Index demu to no [[Begin], [End]] roduk, osem ela [Index]."},
		{"error.index.type", "Index 操作只接受 Integer 型或仅包含 Integer 型的 Array 型作为输入。"},
		{"error.index.incorrectRange.string", "对 String 型实施 Index 操作时遇到了宽度为负的区间：[[Start], [End]]。"},
		{"error.index.stringAt1", "String 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [[I0], [I1]]。"},
		{"error.index.arrayAt", "Array 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [Index]。"},
		{"error.index.incorrectRange.array", "对 Array 型实施 Index 操作时遇到了宽度为负的区间：[[Start], [End]]。"},
		{"error.index.arrayAt1", "Array 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [[I0], [I1]]。"},
		{"error.index.typeSupport", "Index 操作只支持对 Array 型和 String 型使用。"},
		{"error.infix.notIdentifier", "成员必须通过 Identifier 进行描述。"}
	};
}