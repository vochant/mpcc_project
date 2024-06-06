#pragma once

#include "i18n/i18n.hpp"

// will be translated to Module.

Language get_i18i_ma_MP() {
	return Language {
		{"key": "lang.name", "value": "Minffosa (Minftis Dubehs)"}
		{"key": "error.call.tooLessArgs", "value": "Juge tos ale tos fë [Expected] srodukya, osem güma ela tos [Current] srodukya."}
		{"key": "error.call.incorrectArgs", "value": "Juge ale tos [Expected] srodukya, osem ela tos [Current] srodukya."}
		{"key": "error.call.nativeFormat", "value": "Ovale feve kadele tos letomip."}
		{"key": "error.call.nativeUnhandled", "value": "Ovale feve kadele tos upmip."}
		{"key": "error.ast.program", "value": "Otaida kadeler leto! Agsil os un el kadele idept MPCC Project i un elr lorof sil jon"}
		{"key": "error.evaluator.moreExpect", "value": "Evak idept etpä to srodukya id os Array pato, ag id os [Type] pato."}
		{"key": "error.evaluator.constructorType", "value": "Tumak sïdettole Function pato."}
		{"key": "error.ast.functionInner", "value": "Tumak sFunction pator Inner erdukya ScopeNode pato."}
		{"key": "error.call.notExecutable", "value": "Os[Subject] re sil ettole io feve ettole."}
		{"key": "error.call.notExecutable1", "value": "Osel re sil ettole io feve ettole."}
		{"key": "error.evaluator.nullAssignUnexpected", "value": "Konö etduk o Null pato."}
		{"key": "error.call.xcrementArgs", "value": "Ota id Increment ettole io Decrement ettole to srodukya."}
		{"key": "error.call.nativeFormat1", "value": "Ovale no [Subject] feve kadele tos letomip."}
		{"key": "error.call.nativeUnhandled1", "value": "Ovale no [Subject] feve kadele tos upmip."}
		{"key": "error.call.indexArgs", "value": "Tumak ag id Index ettole to re re srodukya i ota id Srodukya Etpä."}
		{"key": "error.index.stringAt", "value": "Juge ale String pator Index demu to no [[Begin], [End]] roduk, osem ela [Index]."}
		{"key": "error.index.type", "value": "Index 操作只接受 Integer 型或仅包含 Integer 型的 Array 型作为输入。"}
		{"key": "error.index.incorrectRange.string", "value": "对 String 型实施 Index 操作时遇到了宽度为负的区间：[[Start], [End]]。"}
		{"key": "error.index.stringAt1", "value": "String 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [[I0], [I1]]。"}
		{"key": "error.index.arrayAt", "value": "Array 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [Index]。"}
		{"key": "error.index.incorrectRange.array", "value": "对 Array 型实施 Index 操作时遇到了宽度为负的区间：[[Start], [End]]。"}
		{"key": "error.index.arrayAt1", "value": "Array 型 Index 操作期望获得 [[Begin], [End]] 之间的下标，实际获得 [[I0], [I1]]。"}
		{"key": "error.index.typeSupport", "value": "Index 操作只支持对 Array 型和 String 型使用。"}
		{"error.infix.notIdentifier", "成员必须通过 Identifier 进行描述。"}
	};
}