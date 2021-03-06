/**Copyright (C) Austin Hicks, 2014-2016
This file is part of Libaudioverse, a library for realtime audio applications.
This code is dual-licensed.  It is released under the terms of the Mozilla Public License version 2.0 or the Gnu General Public License version 3 or later.
You may use this code under the terms of either license at your option.
A copy of both licenses may be found in license.gpl and license.mpl at the root of this repository.
If these files are unavailable to you, see either http://www.gnu.org/licenses/ (GPL V3 or later) or https://www.mozilla.org/en-US/MPL/2.0/ (MPL 2.0).*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/nodes/one_pole_filter.hpp>
#include <libaudioverse/implementations/one_pole_filter.hpp>
#include <libaudioverse/private/server.hpp>
#include <libaudioverse/private/node.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/private/multichannel_filter_bank.hpp>
#include <memory>

namespace libaudioverse_implementation {

OnePoleFilterNode::OnePoleFilterNode(std::shared_ptr<Server> s, int channels): Node(Lav_OBJTYPE_ONE_POLE_FILTER_NODE, s, channels, channels),
bank(server->getSr()) {
	if(channels < 1) ERROR(Lav_ERROR_RANGE, "Cannot filter 0 or fewer channels.");
	bank.setChannelCount(channels);
	getProperty(Lav_ONE_POLE_FILTER_FREQUENCY).setFloatRange(0, server->getSr()/2.0);
	reconfigureFilters();
	appendInputConnection(0, channels);
	appendOutputConnection(0, channels);
	setShouldZeroOutputBuffers(false);
}

std::shared_ptr<Node> createOnePoleFilterNode(std::shared_ptr<Server> server, int channels) {
	return standardNodeCreation<OnePoleFilterNode>(server, channels);
}

void OnePoleFilterNode::reconfigureFilters() {
	float freq = getProperty(Lav_ONE_POLE_FILTER_FREQUENCY).getFloatValue();
	bool isHighpass = getProperty(Lav_ONE_POLE_FILTER_IS_HIGHPASS).getIntValue() == 1; //==1 prevents a performance warning from VC++.
	bank->setPoleFromFrequency(freq, isHighpass);
}

void OnePoleFilterNode::process() {
	if(werePropertiesModified(this, Lav_ONE_POLE_FILTER_IS_HIGHPASS, Lav_ONE_POLE_FILTER_FREQUENCY)) reconfigureFilters();
	auto &freqProp = getProperty(Lav_ONE_POLE_FILTER_FREQUENCY);
	bool isHighpass = getProperty(Lav_ONE_POLE_FILTER_IS_HIGHPASS).getIntValue() == 1;
	if(freqProp.needsARate()) bank.process(block_size, &input_buffers[0], &output_buffers[0], [&] (OnePoleFilter& filter, int index) {
		filter.setPoleFromFrequency(freqProp.getFloatValue(index), isHighpass);
	});
	else bank.process(block_size, &input_buffers[0], &output_buffers[0]);
}

//begin public api.

Lav_PUBLIC_FUNCTION LavError Lav_createOnePoleFilterNode(LavHandle serverHandle, int channels, LavHandle* destination) {
	PUB_BEGIN
	auto server = incomingObject<Server>(serverHandle);
	LOCK(*server);
	auto retval= createOnePoleFilterNode(server, channels);
	*destination =outgoingObject<Node>(retval);
	PUB_END
}

}