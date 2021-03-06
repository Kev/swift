/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/HighLayer.h>

#include <cassert>

#include <Swiften/StreamStack/LowLayer.h>

namespace Swift {

HighLayer::HighLayer() : childLayer(NULL) {
}

HighLayer::~HighLayer() {
}

void HighLayer::writeDataToChildLayer(const SafeByteArray& data) {
	//assert(childLayer);
	if (childLayer) {
		childLayer->writeData(data);
	}
}

}
