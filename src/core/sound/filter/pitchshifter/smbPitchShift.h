#ifndef smbPitchShiftH
#define smbPitchShiftH



class smbPitchShifter
{
public:
	static const size_t MAX_FRAME_LENGTH = 8192;
private:
	float gInFIFO[MAX_FRAME_LENGTH];
	float gOutFIFO[MAX_FRAME_LENGTH];
	float gFFTworksp[2*MAX_FRAME_LENGTH];
	float gLastPhase[MAX_FRAME_LENGTH/2+1];
	float gSumPhase[MAX_FRAME_LENGTH/2+1];
	float gOutputAccum[2*MAX_FRAME_LENGTH];
	float gAnaFreq[MAX_FRAME_LENGTH];
	float gAnaMagn[MAX_FRAME_LENGTH];
	float gSynFreq[MAX_FRAME_LENGTH];
	float gSynMagn[MAX_FRAME_LENGTH];
	long gRover;

	float gInputBuffer[MAX_FRAME_LENGTH];
	float gOutputBuffer[MAX_FRAME_LENGTH];

public:
	smbPitchShifter();
	void filter(float pitchShift,
		long numSampsToProcess,
		long fftFrameSize,
		long osamp,
		float sampleRate);

	float * GetInputBuffer() { return gInputBuffer; }
	float * GetOutputBuffer() { return gOutputBuffer; }

private:
	static double atan2(double x, double y);
	static void fft(float *fftBuffer, long fftFrameSize, long sign);
};





#endif

