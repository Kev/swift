/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/IQ.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

boost::shared_ptr<IQ> IQ::createRequest(
		Type type, const JID& to, const std::string& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq = boost::make_shared<IQ>(type);
	if (to.isValid()) {
		iq->setTo(to);
	}
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createResult(const JID& to, const std::string& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq = boost::make_shared<IQ>(Result);
	iq->setTo(to);
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createResult(const JID& to, const JID& from, const std::string& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq = boost::make_shared<IQ>(Result);
	iq->setTo(to);
	iq->setFrom(from);
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createError(const JID& to, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq = boost::make_shared<IQ>(IQ::Error);
	iq->setTo(to);
	iq->setID(id);
	boost::shared_ptr<ErrorPayload> errorPayload = boost::make_shared<Swift::ErrorPayload>(condition, type);
	errorPayload->setPayload(payload);
	iq->addPayload(errorPayload);
	return iq;
}

boost::shared_ptr<IQ> IQ::createError(const JID& to, const JID& from, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq = boost::make_shared<IQ>(IQ::Error);
	iq->setTo(to);
	iq->setFrom(from);
	iq->setID(id);
	boost::shared_ptr<ErrorPayload> errorPayload = boost::make_shared<Swift::ErrorPayload>(condition, type);
	errorPayload->setPayload(payload);
	iq->addPayload(errorPayload);
	return iq;
}

}
