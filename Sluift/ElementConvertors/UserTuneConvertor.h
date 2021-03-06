/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/UserTune.h>

namespace Swift {
	class LuaElementConvertors;

	class UserTuneConvertor : public GenericLuaElementConvertor<UserTune> {
		public:
			UserTuneConvertor(LuaElementConvertors* convertors);
			virtual ~UserTuneConvertor();

			virtual boost::shared_ptr<UserTune> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<UserTune>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
