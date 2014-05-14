#include <libaudioverse/private_all.h>
#include <portaudio.h>
#include <uthash.h>
#include <stdio.h>

typedef struct {
	LavGraph *graph;
	LavCrossThreadRingBuffer *ring_buffer;
	PaStream *stream;
	unsigned int block_size, mix_ahead, channels;
	void* running_flag; //when cleared, the thread dies.
} ThreadParams;

int audioOutputCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
void audioOutputThread(void* vparam);

unsigned int thread_counter = 0; //Used for portaudio init and deinit.  When decremented to 0, deinit portaudio.

Lav_PUBLIC_FUNCTION LavError createAudioOutputThread(LavGraph *graph, unsigned int blockSize, unsigned int mixAhead, void **destination) {
	mixAhead+=1; //so we can actually mixahead 0 times.
	WILL_RETURN(LavError);
	if(thread_counter == 0) {
		PaError e = Pa_Initialize();
		ERROR_IF_TRUE(e != paNoError, Lav_ERROR_CANNOT_INIT_AUDIO);
	}
	thread_counter ++;
	CHECK_NOT_NULL(graph);
	LOCK(graph->mutex);
	ThreadParams *param = calloc(1, sizeof(ThreadParams));
	param->graph = graph;
	LavError err;
	err = createCrossThreadRingBuffer(blockSize*mixAhead, sizeof(float), &param->ring_buffer);
	ERROR_IF_TRUE(err != Lav_ERROR_NONE, err);
	param->block_size = blockSize;
	param->mix_ahead = mixAhead;

	//Calculate the number of channels.
	LavNode *node;
	Lav_graphGetOutputNode(graph, &node);
	unsigned int channels = node->num_outputs;
	PaStream *stream;
	Pa_OpenDefaultStream(&stream, 0, channels, paFloat32, graph->sr, blockSize, audioOutputCallback, param);
	param->stream = stream;
	param->channels = channels;

	//Make the flag:
	err = createAFlag(&param->running_flag);
	ERROR_IF_TRUE(err != Lav_ERROR_NONE, err);
	aFlagTestAndSet(param->running_flag);
	void* th;
	err = threadRun(audioOutputThread, param, &th);
	ERROR_IF_TRUE(err != Lav_ERROR_NONE, err);
	//let it go!
	*destination = param;
	RETURN(Lav_ERROR_NONE);
	STANDARD_CLEANUP_BLOCK(graph->mutex);
}

Lav_PUBLIC_FUNCTION void stopAudioOutputThread(void* thread) {
	aFlagClear(((ThreadParams*)thread)->running_flag);
}

void audioOutputThread(void* vparam) {
	ThreadParams *param = (ThreadParams*)vparam;
	LavGraph *graph = param->graph;
	LavCrossThreadRingBuffer *rb = param->ring_buffer;
	Pa_StartStream(param->stream);
	//This is simple.
	//Process one block from the graph, write it to the ringbuffer, repeat.
	float* samples = malloc(param->block_size*sizeof(float)*param->channels);
	while(aFlagTestAndSet(param->running_flag)) {
		memset(samples, 0, param->block_size*sizeof(float)*param->channels);
		Lav_graphReadAllOutputs(graph, param->block_size, samples);
		CTRBWriteItems(rb, param->block_size*param->channels, samples);
		while(CTRBGetAvailableWrites(rb) <= param->block_size);// sleepFor(1); //sleep for 1 ms.
	}
}

int audioOutputCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	//read length items into the output buffer.  That is it.
	CTRBGetItems(((ThreadParams*)userData)->ring_buffer, frameCount, output);
	return paNoError;
}