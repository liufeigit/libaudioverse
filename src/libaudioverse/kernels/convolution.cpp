/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/

/**Implements the convolution kernel.*/
#include <libaudioverse/private_kernels.hpp>


void convolutionKernel(float* input, unsigned int outputSampleCount, float* output, unsigned int responseLength, float* response) {
	for(unsigned int i = 0; i < outputSampleCount; i++) {
		float samp = 0.0f;
		for(unsigned int j = 0; j < responseLength; j++) {
			samp += input[i+j]*response[responseLength-j-1];
		}
		output[i] = samp;
	}
}


void crossfadeConvolutionKernel(float* input, unsigned int outputSampleCount, float* output, unsigned int responseLength, float* from, float* to) {
	float delta = 1.0f/outputSampleCount;
	for(unsigned int i = 0; i < outputSampleCount; i++) {
		float weight1 = 1.0f-delta*i;
		float weight2 = i*delta;
		float samp = 0.0f;
		for(unsigned int j = 0; j < responseLength; j++) {
			samp += input[i + j] * (weight1*from[responseLength-j-1] + weight2*to[responseLength-j-1]);
		}
	output[i] = samp;
	}
}