#pragma once

#include <functional>

#include "i18n/i18n.hpp"
#include "i18n/zh_CN.hpp"

I18N i18n;

void InitI18NWith(std::function<Language()> getter) {
	auto val = getter();
	i18n.Register(val);
}

void InitI18N() {
	switch (I18N::DEFAULT_I18N) {
	case I18N::zh_CN:
		InitI18NWith(get_i18n_zh_CN);
		break;
	}
}