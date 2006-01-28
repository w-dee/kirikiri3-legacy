//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder �̎���
//---------------------------------------------------------------------------
#ifndef RisaPhaseVocoderH
#define RisaPhaseVocoderH

#include "RingBuffer.h"

//---------------------------------------------------------------------------
//! @brief Phase Vocoder DSP �N���X
//---------------------------------------------------------------------------
class tRisaPhaseVocoderDSP
{
	const static float MAX_TIME_SCALE = 1.95; //!< �ő�� time scale �l
	const static float MIN_TIME_SCALE = 0.25; //!< �ŏ��� time scale �l

	float * AnalWork; //!< ���(Analyze)�p�o�b�t�@(FrameSize��) ���O�ŏ΂�Ȃ��悤��
	float * SynthWork; //!< �����p��ƃo�b�t�@(FrameSize)
	float ** LastAnalPhase; //!< �O���͎��̊e�t�B���^�o���h�̈ʑ� (�e�`�����l�����Ƃ�FrameSize/2��)
	float ** LastSynthPhase; //!< �O�񍇐����̊e�t�B���^�o���h�̈ʑ� (�e�`�����l�����Ƃ�FrameSize/2��)

	int * FFTWorkIp; //!< rdft �ɓn�� ip �p�����[�^
	float * FFTWorkW; //!< rdft �ɓn�� w �p�����[�^
	float * InputWindow; //!< ���͗p���֐�
	float * OutputWindow; //!< �o�͗p���֐�

	unsigned int FrameSize; //!< FFT�T�C�Y
	unsigned int OverSampling; //!< �I�[�o�[�E�T���v�����O�W��
	unsigned int Frequency; //!< PCM �T���v�����O���g��
	unsigned int Channels; //!< PCM �`�����l����
	unsigned int InputHopSize; //!< FrameSize/OverSampling

	tRisaRingBuffer<float> InputBuffer; //!< ���͗p�����O�o�b�t�@
	tRisaRingBuffer<float> OutputBuffer; //!< �o�͗p�����O�o�b�t�@

	float	TimeScale; //!< ���Ԏ������̃X�P�[��(�o��/����)
	float	FrequencyScale; //!< ���g�������̃X�P�[��(�o��/����)

	bool	RebuildParams; //!< �����I�ȃp�����[�^�Ȃǂ��č\�z���Ȃ���΂Ȃ�Ȃ��Ƃ��ɐ^

	// �ȉ��ARebuildParams ���^�̎��ɍč\�z�����p�����[�^
	// �����ɂ��郁���o�ȊO�ł́AInputWindow �� OutputWindow ���č\�z�����
	unsigned int OutputHopSize; //!< InputHopSize * TimeScale
	float OverSamplingRadian; //!< (2.0*M_PI)/OverSampling
	float OverSamplingRadianRecp; //!< OverSamplingRadian �̋t��
	float FrequencyPerFilterBand; //!< Frequency/FrameSize
	float FrequencyPerFilterBandRecp; //!< FrequencyPerFilterBand �̋t��
	float ExactTimeScale; //!< ������TimeScale = OutputHopSize / InputHopSize

public:
	//! @brief Process ���Ԃ��X�e�[�^�X
	enum tStatus
	{
		psNoError, //!< ���Ȃ�
		psInputNotEnough, //!< ���͂������Ȃ� (GetInputBuffer�œ����|�C���^�ɏ����Ă���Ď��s����)
		psOutputFull //!< �o�̓o�b�t�@�������ς� (GetOutputBuffer�œ����|�C���^����ǂݏo���Ă���Ď��s����)
	};

public:
	tRisaPhaseVocoderDSP(unsigned int framesize, unsigned int oversamp,
					unsigned int frequency, unsigned int channels);
	~tRisaPhaseVocoderDSP();

	float GetTimeScale() const { return TimeScale; } //!< ���Ԏ������̃X�P�[���𓾂�
	void SetTimeScale(float v);

	float GetFrequencyScale() const { return FrequencyScale; } //!< ���g���������̃X�P�[���𓾂�
	void SetFrequencyScale(float v);

private:
	void Clear();

public:
	size_t GetInputFreeSize();
	bool GetInputBuffer(size_t numsamplegranules,
		float * & p1, size_t & p1size,
		float * & p2, size_t & p2size);

	size_t GetOutputReadySize();
	bool GetOutputBuffer(size_t numsamplegranules,
		const float * & p1, size_t & p1size,
		const float * & p2, size_t & p2size);

	tStatus Process();

private:
	void Deinterleave(float * dest, const float * src,
					float * win, size_t len);

	void Interleave(float * dest, const float * src,
					float * win, size_t len);

};
//---------------------------------------------------------------------------

#endif
