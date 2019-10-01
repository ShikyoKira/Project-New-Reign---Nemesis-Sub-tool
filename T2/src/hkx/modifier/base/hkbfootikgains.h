#ifndef HKBFOOTIKGAINS_H_
#define HKBFOOTIKGAINS_H_

struct gain
{
	double onOffGain;
	double groundAscendingGain;
	double groundDescendingGain;
	double footPlantedGain;
	double footRaisedGain;
	double footUnlockGain;
	double worldFromModelFeedbackGain;
	double errorUpDownBias;
	double alignWorldFromModelGain;
	double hipOrientationGain;
	double maxKneeAngleDifference;
	double ankleOrientationGain;
};

#endif
