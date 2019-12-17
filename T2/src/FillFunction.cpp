#include <boost\thread.hpp>
#include <boost\regex.hpp>
#include "FillFunction.h"
#include "AnimData\animationdata.h"
#include "AnimSetData\animationsetdata.h"

using namespace std;

shared_ptr<hkbobject> GetHKXClass(string classname, string id);
void dataFill(shared_ptr<hkbobject> hkb_obj, string filename, vecstr buffer, bool isEdited);

void AddBehavior(string filename, vecstr& storeline, bool edited)
{
	string classname;
	string tempID;
	string idfront = "<hkobject name=\"";
	string classfront = "\" class=\"";
	string classback = "\" signature=\"";
	vecstr buffer;
	// boost::thread_group multi_t;		removed due to it being ineffective and increase process time instead
	shared_ptr<hkbobject> node;
	hkRefPtr* curBehavior = edited ? &editedBehavior : &originalBehavior;
	unordered_map<string, vecstr> nodeStore;
	bool virgin = true;
	bool record = false;
	bool eventOpen = false;
	bool attriOption = false;
	bool varOpen = false;
	bool charOpen = false;
	int counter = 0;
	buffer.reserve(10000);

	for (auto& line: storeline)
	{
		if (Error)
		{
			return;
		}
		
		if (line.find("SERIALIZE_IGNORED", 0) == string::npos)
		{
			if (edited && line.find("#") != string::npos)
			{
				usize lastpos = 0;
				string templine;

				// changing number to the ID to avoid conflict
				for (boost::sregex_iterator& itr = boost::sregex_iterator(line.begin(), line.end(), boost::regex("(?<!&)#(\\d+?)(?![\\d;])")); itr != boost::sregex_iterator(); ++itr)
				{
					string curID = itr->str(1);
					usize pos = itr->position(1);
					templine.append(line.substr(lastpos, pos - lastpos) + "9" + curID);
					lastpos = pos + curID.length();
				}

				if (templine.length() > 0 && lastpos != 0) line = templine + line.substr(lastpos);
			}

			if (!record)
			{
				usize pos = line.find(idfront);

				if (pos != string::npos)
				{
					usize pos2 = line.find(classfront, pos);

					if (pos2 != string::npos)
					{
						pos += idfront.length();
						string ID = line.substr(pos, pos2 - pos);
						usize pos3 = line.find(classback, pos2);
						
						if (pos3 != string::npos)
						{
							if (!virgin)
							{
								(*curBehavior)[tempID] = node;
								node->regis(tempID, edited);
								
								if (node->getClassCode() == "b") node->dataBake(filename, buffer, edited);
								else nodeStore[tempID] = buffer;

								// multi_t.create_thread(boost::bind(&dataFill, node, filename, buffer, edited));
							}

							buffer.clear();
							pos2 += classfront.length();
							classname = line.substr(pos2, pos3 - pos2);
							tempID = ID;
							node = GetHKXClass(classname, tempID);
							record = true;
							virgin = false;
						}
					}
				}
			}

			if (line.length() == 0)
			{
				record = false;
			}

			if (record)
			{
				// !edited ? FunctionLineOriginal[tempID].push_back(line) : FunctionLineEdited[tempID].push_back(line);
				buffer.push_back(line);
			}
		}
	}

	if (!virgin)
	{
		(*curBehavior)[tempID] = node;
		node->regis(tempID, edited);
		nodeStore[tempID] = buffer;
		// node->dataBake(filename, buffer, edited);
	}

	for (auto& node : nodeStore)
	{
		(*curBehavior)[node.first]->dataBake(filename, node.second, edited);
	}

	// multi_t.join_all();
}

void dataFill(shared_ptr<hkbobject> hkb_obj, string filename, vecstr buffer, bool isEdited)
{
	hkb_obj->dataBake(filename, buffer, isEdited);
}

void AddAnimData(string filename, vecstr& storeline, bool edited)
{
	if (edited)
	{
		animDataInitialize(filename, storeline, AnimDataEdited);
	}
	else
	{
		animDataInitialize(filename, storeline, AnimDataOriginal);
	}
}

void AddAnimSetData(string filename, vecstr& storeline, bool edited)
{
	if (edited)
	{
		animSetDataInitialize(filename, storeline, AnimSetDataEdited);
	}
	else
	{
		animSetDataInitialize(filename, storeline, AnimSetDataOriginal);
	}
}

shared_ptr<hkbobject> GetHKXClass(string classname, string id)
{
	if (classname == "BSBoneSwitchGeneratorBoneData")
	{
		return make_shared<bsboneswitchgeneratorbonedata>();
	}
	else if (classname == "BSBoneSwitchGenerator")
	{
		return make_shared<bsboneswitchgenerator>();
	}
	else if (classname == "BSCyclicBlendTransitionGenerator")
	{
		return make_shared<bscyclicblendtransitiongenerator>();
	}
	else if (classname == "BSiStateTaggingGenerator")
	{
		return make_shared<bsistatetagginggenerator>();
	}
	else if (classname == "hkbBehaviorGraphStringData")
	{
		return make_shared<hkbbehaviorgraphstringdata>();
	}
	else if (classname == "hkbBehaviorGraphData")
	{
		return make_shared<hkbbehaviorgraphdata>();
	}
	else if (classname == "hkbBehaviorGraph")
	{
		return make_shared<hkbbehaviorgraph>();
	}
	else if (classname == "hkbBlenderGeneratorChild")
	{
		return make_shared<hkbblendergeneratorchild>();
	}
	else if (classname == "hkbBlenderGenerator")
	{
		return make_shared<hkbblendergenerator>();
	}
	else if (classname == "hkbBlendingTransitionEffect")
	{
		return make_shared<hkbblendingtransitioneffect>();
	}
	else if (classname == "hkbBoneWeightArray")
	{
		return make_shared<hkbboneweightarray>();
	}
	else if (classname == "hkbExpressionCondition")
	{
		return make_shared<hkbexpressioncondition>();
	}
	else if (classname == "hkbManualSelectorGenerator")
	{
		return make_shared<hkbmanualselectorgenerator>();
	}
	else if (classname == "hkRootLevelContainer")
	{
		return make_shared<hkrootlevelcontainer>();
	}
	else if (classname == "hkbStateMachineEventPropertyArray")
	{
		return make_shared<hkbstatemachineeventpropertyarray>();
	}
	else if (classname == "hkbStateMachineStateInfo")
	{
		return make_shared<hkbstatemachinestateinfo>();
	}
	else if (classname == "hkbStateMachineTransitionInfoArray")
	{
		return make_shared<hkbstatemachinetransitioninfoarray>();
	}
	else if (classname == "hkbStateMachine")
	{
		return make_shared<hkbstatemachine>();
	}
	else if (classname == "hkbStringEventPayload")
	{
		return make_shared<hkbstringeventpayload>();
	}
	else if (classname == "hkbVariableBindingSet")
	{
		return make_shared<hkbvariablebindingset>();
	}
	else if (classname == "hkbVariableValueSet")
	{
		return make_shared<hkbvariablevalueset>();
	}
	else if (classname == "hkbClipGenerator")
	{
		return make_shared<hkbclipgenerator>();
	}
	else if (classname == "hkbClipTriggerArray")
	{
		return make_shared<hkbcliptriggerarray>();
	}
	else if (classname == "hkbBehaviorReferenceGenerator")
	{
		return make_shared<hkbbehaviorreferencegenerator>();
	}
	else if (classname == "hkbModifierGenerator")
	{
		return make_shared<hkbmodifiergenerator>();
	}
	else if (classname == "hkbModifierList")
	{
		return make_shared<hkbmodifierlist>();
	}
	else if (classname == "BSSynchronizedClipGenerator")
	{
		return make_shared<bssynchronizedclipgenerator>();
	}
	else if (classname == "hkbTwistModifier")
	{
		return make_shared<hkbtwistmodifier>();
	}
	else if (classname == "hkbEventDrivenModifier")
	{
		return make_shared<hkbeventdrivenmodifier>();
	}
	else if (classname == "BSIsActiveModifier")
	{
		return make_shared<bsisactivemodifier>();
	}
	else if (classname == "BSLimbIKModifier")
	{
		return make_shared<bslimbikmodifier>();
	}
	else if (classname == "BSInterpValueModifier")
	{
		return make_shared<bsinterpvaluemodifier>();
	}
	else if (classname == "BSGetTimeStepModifier")
	{
		return make_shared<bsgettimestepmodifier>();
	}
	else if (classname == "hkbFootIkControlsModifier")
	{
		return make_shared<hkbfootikcontrolsmodifier>();
	}
	else if (classname == "hkbGetHandleOnBoneModifier")
	{
		return make_shared<hkbgethandleonbonemodifier>();
	}
	else if (classname == "hkbTransformVectorModifier")
	{
		return make_shared<hkbtransformvectormodifier>();
	}
	else if (classname == "hkbProxyModifier")
	{
		return make_shared<hkbproxymodifier>();
	}
	else if (classname == "hkbLookAtModifier")
	{
		return make_shared<hkblookatmodifier>();
	}
	else if (classname == "hkbMirrorModifier")
	{
		return make_shared<hkbmirrormodifier>();
	}
	else if (classname == "hkbGetWorldFromModelModifier")
	{
		return make_shared<hkbgetworldfrommodelmodifier>();
	}
	else if (classname == "hkbSenseHandleModifier")
	{
		return make_shared<hkbsensehandlemodifier>();
	}
	else if (classname == "hkbEvaluateExpressionModifier")
	{
		return make_shared<hkbevaluateexpressionmodifier>();
	}
	else if (classname == "hkbExpressionDataArray")
	{
		return make_shared<hkbexpressiondataarray>();
	}
	else if (classname == "hkbEvaluateHandleModifier")
	{
		return make_shared<hkbevaluatehandlemodifier>();
	}
	else if (classname == "hkbAttachmentModifier")
	{
		return make_shared<hkbattachmentmodifier>();
	}
	else if (classname == "hkbAttributeModifier")
	{
		return make_shared<hkbattributemodifier>();
	}
	else if (classname == "hkbCombineTransformsModifier")
	{
		return make_shared<hkbcombinetransformsmodifier>();
	}
	else if (classname == "hkbComputeRotationFromAxisAngleModifier")
	{
		return make_shared<hkbcomputerotationfromaxisanglemodifier>();
	}
	else if (classname == "hkbComputeRotationToTargetModifier")
	{
		return make_shared<hkbcomputerotationtotargetmodifier>();
	}
	else if (classname == "hkbEventsFromRangeModifier")
	{
		return make_shared<hkbeventsfromrangemodifier>();
	}
	else if (classname == "hkbEventRangeDataArray")
	{
		return make_shared<hkbeventrangedataarray>();
	}
	else if (classname == "hkbMoveCharacterModifier")
	{
		return make_shared<hkbmovecharactermodifier>();
	}
	else if (classname == "hkbExtractRagdollPoseModifier")
	{
		return make_shared<hkbextractragdollposemodifier>();
	}
	else if (classname == "BSModifyOnceModifier")
	{
		return make_shared<bsmodifyoncemodifier>();
	}
	else if (classname == "BSEventOnDeactivateModifier")
	{
		return make_shared<bseventondeactivatemodifier>();
	}
	else if (classname == "BSEventEveryNEventsModifier")
	{
		return make_shared<bseventeveryneventsmodifier>();
	}
	else if (classname == "BSRagdollContactListenerModifier")
	{
		return make_shared<bsragdollcontactlistenermodifier>();
	}
	else if (classname == "hkbPoweredRagdollControlsModifier")
	{
		return make_shared<hkbpoweredragdollcontrolmodifier>();
	}
	else if (classname == "BSEventOnFalseToTrueModifier")
	{
		return make_shared<bseventonfalsetotruemodifier>();
	}
	else if (classname == "BSDirectAtModifier")
	{
		return make_shared<bsdirectatmodifier>();
	}
	else if (classname == "BSDistTriggerModifier")
	{
		return make_shared<bsdisttriggermodifier>();
	}
	else if (classname == "BSDecomposeVectorModifier")
	{
		return make_shared<bsdecomposevectormodifier>();
	}
	else if (classname == "BSComputeAddBoneAnimModifier")
	{
		return make_shared<bscomputeaddboneanimmodifier>();
	}
	else if (classname == "BSTweenerModifier")
	{
		return make_shared<bstweenermodifier>();
	}
	else if (classname == "BSIStateManagerModifier")
	{
		return make_shared<bsistatemanagermodifier>();
	}
	else if (classname == "hkbTimerModifier")
	{
		return make_shared<hkbtimermodifier>();
	}
	else if (classname == "hkbRotateCharacterModifier")
	{
		return make_shared<hkbrotatecharactermodifier>();
	}
	else if (classname == "hkbDampingModifier")
	{
		return make_shared<hkbdampingmodifier>();
	}
	else if (classname == "hkbDelayedModifier")
	{
		return make_shared<hkbdelayedmodifier>();
	}
	else if (classname == "hkbGetUpModifier")
	{
		return make_shared<hkbgetupmodifier>();
	}
	else if (classname == "hkbKeyframeBonesModifier")
	{
		return make_shared<hkbkeyframebonesmodifier>();
	}
	else if (classname == "hkbBoneIndexArray")
	{
		return make_shared<hkbboneindexarray>();
	}
	else if (classname == "hkbComputeDirectionModifier")
	{
		return make_shared<hkbcomputedirectionmodifier>();
	}
	else if (classname == "hkbRigidBodyRagdollControlsModifier")
	{
		return make_shared<hkbrigidbodyragdollcontrolsmodifier>();
	}
	else if (classname == "BSSpeedSamplerModifier")
	{
		return make_shared<bsspeedsamplermodifier>();
	}
	else if (classname == "hkbDetectCloseToGroundModifier")
	{
		return make_shared<hkbdetectclosetogroundmodifier>();
	}
	else if (classname == "BSLookAtModifier")
	{
		return make_shared<bslookatmodifier>();
	}
	else if (classname == "BSTimerModifier")
	{
		return make_shared<bstimermodifier>();
	}
	else if (classname == "BSPassByTargetTriggerModifier")
	{
		return make_shared<bspassbytargettriggermodifier>();
	}
	else if (classname == "hkbHandIkControlsModifier")
	{
		return make_shared<hkbhandikcontrolsmodifier>();
	}
	else if (classname == "hkbMirroredSkeletonInfo")
	{
		return make_shared<hkbmirroredskeletoninfo>();
	}
	else if (classname == "hkbCharacterStringData")
	{
		return make_shared<hkbcharacterstringdata>();
	}
	else if (classname == "hkbFootIkDriverInfo")
	{
		return make_shared<hkbfootikdriverinfo>();
	}
	else if (classname == "hkbHandIkDriverInfo")
	{
		return make_shared<hkbhandikdriverinfo>();
	}
	else if (classname == "hkbCharacterData")
	{
		return make_shared<hkbcharacterdata>();
	}
	else if (classname == "BSOffsetAnimationGenerator")
	{
		return make_shared<bsoffsetanimationgenerator>();
	}
	else if (classname == "hkbPoseMatchingGenerator")
	{
		return make_shared<hkbposematchinggenerator>();
	}
	else if (classname == "hkbStringCondition")
	{
		return make_shared<hkbstringcondition>();
	}
	else if (classname == "hkbStateListener")
	{
		return make_shared<hkbstatelistener>();
	}
	else if (classname == "hkbStateChooser")
	{
		return make_shared<hkbstatechooser>();
	}
	else if (classname == "hkbHandle")
	{
		return make_shared<hkbhandle>();
	}
	else if (classname == "hkbAttachmentSetup")
	{
		return make_shared<hkbattachmentsetup>();
	}
	else if (classname == "hkbGeneratorTransitionEffect")
	{
		return make_shared<hkbgeneratortransitioneffect>();
	}
	else if (classname == "hkbFootIkModifier")
	{
		return make_shared<hkbfootikmodifier>();
	}
	else
	{
		cout << "ERROR: Edit Class Not Found (Classname: " << classname << ", ID: " << id << ")" << endl;
		Error = true;
		throw false;
	}
}
