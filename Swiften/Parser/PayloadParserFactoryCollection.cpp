/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <algorithm>

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>

namespace Swift {

PayloadParserFactoryCollection::PayloadParserFactoryCollection() : defaultFactory_(NULL) {
}

void PayloadParserFactoryCollection::addFactory(PayloadParserFactory* factory) {
	factories_.push_back(factory);
}

void PayloadParserFactoryCollection::removeFactory(PayloadParserFactory* factory) {
	factories_.erase(std::remove(factories_.begin(), factories_.end(), factory), factories_.end());
}

void PayloadParserFactoryCollection::setDefaultFactory(PayloadParserFactory* factory) {
	defaultFactory_ = factory;
}

PayloadParserFactory* PayloadParserFactoryCollection::getPayloadParserFactory(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	std::vector<PayloadParserFactory*>::reverse_iterator i = std::find_if(
			factories_.rbegin(), factories_.rend(),
			boost::bind(&PayloadParserFactory::canParse, _1, element, ns, attributes));
	return (i != factories_.rend() ? *i : defaultFactory_);
}

}
