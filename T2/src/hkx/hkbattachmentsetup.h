#ifndef HKBATTACHMENTSETUP_H_
#define HKBATTACHMENTSETUP_H_

#include "src\hkx\hkbobject.h"

struct hkbattachmentsetup : public hkbobject, std::enable_shared_from_this<hkbattachmentsetup>
{
	hkbattachmentsetup() {}

	enum attachmenttype
	{
		ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY,
		ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT,
		ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT,
		ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL,
		ATTACHMENT_TYPE_NONE
	};

	double blendInTime;
	double moveAttacherFraction;
	double gain;
	double extrapolationTimeStep;
	double fixUpGain;
	double maxLinearDistance;
	double maxAngularDistance;
	attachmenttype attachmentType;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh() {}
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot) {}
	std::string getAttachmentType();
};

extern safeStringUMap<std::shared_ptr<hkbattachmentsetup>> hkbattachmentsetupList;
extern safeStringUMap<std::shared_ptr<hkbattachmentsetup>> hkbattachmentsetupList_E;

#endif
