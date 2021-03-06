/**Copyright (C) Austin Hicks, 2014-2016
This file is part of Libaudioverse, a library for realtime audio applications.
This code is dual-licensed.  It is released under the terms of the Mozilla Public License version 2.0 or the Gnu General Public License version 3 or later.
You may use this code under the terms of either license at your option.
A copy of both licenses may be found in license.gpl and license.mpl at the root of this repository.
If these files are unavailable to you, see either http://www.gnu.org/licenses/ (GPL V3 or later) or https://www.mozilla.org/en-US/MPL/2.0/ (MPL 2.0).*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/implementations/multipanner.hpp>
#include <libaudioverse/nodes/multipanner.hpp>
#include <libaudioverse/private/server.hpp>
#include <libaudioverse/private/node.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/private/constants.hpp>
#include <libaudioverse/private/hrtf.hpp>
#include <memory>


namespace libaudioverse_implementation {

//We always have 8 channels for simplicity.
MultipannerNode::MultipannerNode(std::shared_ptr<Server> server, std::shared_ptr<HrtfData> hrtf): Node(Lav_OBJTYPE_MULTIPANNER_NODE, server, 1, 8),
panner(server->getBlockSize(), server->getSr(), hrtf) {
	appendInputConnection(0, 1);
	appendOutputConnection(0, 2);
	strategyChanged();
}

std::shared_ptr<Node> createMultipannerNode(std::shared_ptr<Server> server, std::shared_ptr<HrtfData> hrtf) {
	auto retval = standardNodeCreation<MultipannerNode>(server, hrtf);
	retval->strategyChanged();
	server->registerNodeForWillTick(retval);
	return retval;
}

void MultipannerNode::strategyChanged() {
	int newStrategy = getProperty(Lav_PANNER_STRATEGY).getIntValue();
	int channels=2;
	switch(newStrategy) {
		case Lav_PANNING_STRATEGY_HRTF:
		case Lav_PANNING_STRATEGY_STEREO:
		channels = 2;
		break;
		case Lav_PANNING_STRATEGY_SURROUND40:
		channels = 4;
		break;
		case Lav_PANNING_STRATEGY_SURROUND51:
		channels=6;
		break;
		case Lav_PANNING_STRATEGY_SURROUND71:
		channels =8;
		break;
	}
	getOutputConnection(0)->reconfigure(0, channels);
}

void MultipannerNode::process() {
	if(werePropertiesModified(this, Lav_PANNER_STRATEGY)) strategyChanged();
	if(werePropertiesModified(this, Lav_PANNER_AZIMUTH)) panner.setAzimuth(getProperty(Lav_PANNER_AZIMUTH).getFloatValue());
	if(werePropertiesModified(this, Lav_PANNER_ELEVATION)) panner.setElevation(getProperty(Lav_PANNER_ELEVATION).getFloatValue());
	if(werePropertiesModified(this, Lav_PANNER_SHOULD_CROSSFADE)) panner.setShouldCrossfade(getProperty(Lav_PANNER_SHOULD_CROSSFADE).getIntValue() == 1);
	if(werePropertiesModified(this, Lav_PANNER_STRATEGY)) panner.setStrategy(getProperty(Lav_PANNER_STRATEGY).getIntValue());
	panner.process(input_buffers[0], &output_buffers[0]);
}

void MultipannerNode::reset() {
}

//begin public api

Lav_PUBLIC_FUNCTION LavError Lav_createMultipannerNode(LavHandle serverHandle, char* hrtfPath, LavHandle* destination) {
	PUB_BEGIN
	auto server = incomingObject<Server>(serverHandle);
	LOCK(*server);
	auto hrtf = createHrtfFromString(hrtfPath, server->getSr());
	*destination = outgoingObject<Node>(createMultipannerNode(server, hrtf));
	PUB_END
}

}