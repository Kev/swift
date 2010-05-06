/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MainWindow.h"

namespace Swift {
	class Roster;
	class MockMainWindow : public MainWindow {
		public:
			MockMainWindow() {};
			virtual ~MockMainWindow() {};
			virtual void setRosterModel(Roster* roster) {this->roster = roster;};
			virtual void setMyName(const String& /*name*/) {};;
			virtual void setMyAvatarPath(const String& /*path*/) {};
			virtual void setMyStatusText(const String& /*status*/) {};
			virtual void setMyStatusType(StatusShow::Type /*type*/) {};
			Roster* roster;

	};
}
