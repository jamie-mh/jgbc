#pragma once

#include "debugger/window.h"
#include "debugger/debugger.h"

class MenuBar final {
	private:
		Debugger &_dbg;

	public:
		MenuBar(Debugger &dbg): _dbg(dbg) { };
		void render() const;
};
