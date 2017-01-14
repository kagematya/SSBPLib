#pragma once
#include "DataArrayReader.h"

namespace ss{

/**
 * EffectCache
 */
class EffectCache
{
public:
	EffectCache()
	{
	}
	~EffectCache()
	{
		releseReference();
	}

	static EffectCache* create(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache)
	{
		EffectCache* obj = new EffectCache();
		if(obj)
		{
			obj->init(data, imageBaseDir, cellCache);
			//			obj->autorelease();
		}
		return obj;
	}

	/**
	* �G�t�F�N�g�t�@�C�������w�肵��EffectRef�𓾂�
	*/
	SsEffectModel* getReference(const std::string& name)
	{
		SsEffectModel* ref = _dic.at(name);
		return ref;
	}

	void dump()
	{
		std::map<std::string, SsEffectModel*>::iterator it = _dic.begin();
		while(it != _dic.end())
		{
			SSLOG("%s", (*it).second);
			++it;
		}
	}
protected:
	void init(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache)
	{
		SS_ASSERT2(data != NULL, "Invalid data");

		ToPointer ptr(data);

		//ssbp����G�t�F�N�g�t�@�C���z����擾
		const EffectFile* effectFileArray = ptr.toEffectFiles(data);

		for(int listindex = 0; listindex < data->numEffectFileList; listindex++)
		{
			//�G�t�F�N�g�t�@�C���z�񂩂�G�t�F�N�g�t�@�C�����擾
			const EffectFile* effectFile = &effectFileArray[listindex];

			//�ێ��p�̃G�t�F�N�g�t�@�C�������쐬
			SsEffectModel *effectmodel = new SsEffectModel();
			std::string effectFileName = ptr.toString(effectFile->name);

			//�G�t�F�N�g�t�@�C������G�t�F�N�g�m�[�h�z����擾
			const EffectNode* effectNodeArray = ptr.toEffectNodes(effectFile);
			for(int nodeindex = 0; nodeindex < effectFile->numNodeList; nodeindex++)
			{
				const EffectNode* effectNode = &effectNodeArray[nodeindex];		//�G�t�F�N�g�m�[�h�z�񂩂�G�t�F�N�g�m�[�h���擾

				SsEffectNode *node = new SsEffectNode();
				node->arrayIndex = effectNode->arrayIndex;
				node->parentIndex = effectNode->parentIndex;
				node->type = (SsEffectNodeType::_enum)effectNode->type;
				node->visible = true;

				SsEffectBehavior behavior;
				//�Z�������쐬
				behavior.CellIndex = effectNode->cellIndex;
				CellRef* cellRef = behavior.CellIndex >= 0 ? cellCache->getReference(behavior.CellIndex) : NULL;
				if(cellRef)
				{
					behavior.refCell.pivot_X = cellRef->cell->pivot_X;
					behavior.refCell.pivot_Y = cellRef->cell->pivot_Y;
					behavior.refCell.texture = cellRef->texture;
					behavior.refCell.texname = cellRef->texname;
					behavior.refCell.rect = cellRef->rect;
					behavior.refCell.cellIndex = behavior.CellIndex;
					std::string name = ptr.toString(cellRef->cell->name);
					behavior.refCell.cellName = name;

				}
				//				behavior.CellName;
				//				behavior.CellMapName;
				behavior.blendType = (SsRenderBlendType::_enum)effectNode->blendType;

				//�G�t�F�N�g�m�[�h����r�w�C�r�A�z����擾
				const ss_offset* behaviorArray = static_cast<const ss_offset*>(ptr(effectNode->Behavior));
				for(int behaviorindex = 0; behaviorindex < effectNode->numBehavior; behaviorindex++)
				{
					//�r�w�C�r�A�z�񂩂�r�w�C�r�A���擾
					const ss_u16* behavior_adr = static_cast<const ss_u16*>(ptr(behaviorArray[behaviorindex]));
					DataArrayReader reader(behavior_adr);

					//�p�����[�^�������push_back�œo�^���Ă���
					int type = reader.readS32();
					switch(type)
					{
					case SsEffectFunctionType::Basic:
					{
						//��{���
						EffectParticleElementBasic readparam;
						readparam.priority = reader.readU32();			//�\���D��x
						readparam.maximumParticle = reader.readU32();		//�ő�p�[�e�B�N����
						readparam.attimeCreate = reader.readU32();		//��x�ɍ쐬����p�[�e�B�N����
						readparam.interval = reader.readU32();			//�����Ԋu
						readparam.lifetime = reader.readU32();			//�G�~�b�^�[��������
						readparam.speedMinValue = reader.readFloat();		//�����ŏ�
						readparam.speedMaxValue = reader.readFloat();		//�����ő�
						readparam.lifespanMinValue = reader.readU32();	//�p�[�e�B�N���������ԍŏ�
						readparam.lifespanMaxValue = reader.readU32();	//�p�[�e�B�N���������ԍő�
						readparam.angle = reader.readFloat();				//�ˏo����
						readparam.angleVariance = reader.readFloat();		//�ˏo�����͈�

						ParticleElementBasic *effectParam = new ParticleElementBasic();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->priority = readparam.priority;							//�\���D��x
						effectParam->maximumParticle = readparam.maximumParticle;			//�ő�p�[�e�B�N����
						effectParam->attimeCreate = readparam.attimeCreate;					//��x�ɍ쐬����p�[�e�B�N����
						effectParam->interval = readparam.interval;							//�����Ԋu
						effectParam->lifetime = readparam.lifetime;							//�G�~�b�^�[��������
						effectParam->speed.setMinMax(readparam.speedMinValue, readparam.speedMaxValue);				//����
						effectParam->lifespan.setMinMax(readparam.lifespanMinValue, readparam.lifespanMaxValue);	//�p�[�e�B�N����������
						effectParam->angle = readparam.angle;								//�ˏo����
						effectParam->angleVariance = readparam.angleVariance;				//�ˏo�����͈�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::RndSeedChange:
					{
						//�V�[�h�㏑��
						EffectParticleElementRndSeedChange readparam;
						readparam.Seed = reader.readU32();				//�㏑������V�[�h�l

						ParticleElementRndSeedChange *effectParam = new ParticleElementRndSeedChange();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Seed = readparam.Seed;							//�㏑������V�[�h�l

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::Delay:
					{
						//�����F�^�C�~���O
						EffectParticleElementDelay readparam;
						readparam.DelayTime = reader.readU32();			//�x������

						ParticleElementDelay *effectParam = new ParticleElementDelay();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->DelayTime = readparam.DelayTime;			//�x������

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::Gravity:
					{
						//�d�͂�������
						EffectParticleElementGravity readparam;
						readparam.Gravity_x = reader.readFloat();			//X�����̏d��
						readparam.Gravity_y = reader.readFloat();			//Y�����̏d��

						ParticleElementGravity *effectParam = new ParticleElementGravity();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Gravity.x = readparam.Gravity_x;			//X�����̏d��
						effectParam->Gravity.y = readparam.Gravity_y;			//Y�����̏d��

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::Position:
					{
						//���W�F������
						EffectParticleElementPosition readparam;
						readparam.OffsetXMinValue = reader.readFloat();	//X���W�ɉ��Z�ŏ�
						readparam.OffsetXMaxValue = reader.readFloat();	//X���W�ɉ��Z�ő�
						readparam.OffsetYMinValue = reader.readFloat();	//X���W�ɉ��Z�ŏ�
						readparam.OffsetYMaxValue = reader.readFloat();	//X���W�ɉ��Z�ő�

						ParticleElementPosition *effectParam = new ParticleElementPosition();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->OffsetX.setMinMax(readparam.OffsetXMinValue, readparam.OffsetXMaxValue); 	//X���W�ɉ��Z�ŏ�
						effectParam->OffsetY.setMinMax(readparam.OffsetYMinValue, readparam.OffsetYMaxValue);	//X���W�ɉ��Z�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::Rotation:
					{
						//Z��]��ǉ�
						EffectParticleElementRotation readparam;
						readparam.RotationMinValue = reader.readFloat();		//�p�x�����l�ŏ�
						readparam.RotationMaxValue = reader.readFloat();		//�p�x�����l�ő�
						readparam.RotationAddMinValue = reader.readFloat();	//�p�x�������Z�l�ŏ�
						readparam.RotationAddMaxValue = reader.readFloat();	//�p�x�������Z�l�ő�

						ParticleElementRotation *effectParam = new ParticleElementRotation();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Rotation.setMinMax(readparam.RotationMinValue, readparam.RotationMaxValue);		//�p�x�����l�ŏ�
						effectParam->RotationAdd.setMinMax(readparam.RotationAddMinValue, readparam.RotationAddMaxValue);	//�p�x�������Z�l�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TransRotation:
					{
						//Z��]���x�ύX
						EffectParticleElementRotationTrans readparam;
						readparam.RotationFactor = reader.readFloat();		//�p�x�ڕW���Z�l
						readparam.EndLifeTimePer = reader.readFloat();		//���B����

						ParticleElementRotationTrans *effectParam = new ParticleElementRotationTrans();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->RotationFactor = readparam.RotationFactor;		//�p�x�ڕW���Z�l
						effectParam->EndLifeTimePer = readparam.EndLifeTimePer;		//���B����

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TransSpeed:
					{
						//���x�F�ω�
						EffectParticleElementTransSpeed readparam;
						readparam.SpeedMinValue = reader.readFloat();			//���x�ڕW�l�ŏ�
						readparam.SpeedMaxValue = reader.readFloat();			//���x�ڕW�l�ő�

						ParticleElementTransSpeed *effectParam = new ParticleElementTransSpeed();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Speed.setMinMax(readparam.SpeedMinValue, readparam.SpeedMaxValue);			//���x�ڕW�l�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TangentialAcceleration:
					{
						//�ڐ������x
						EffectParticleElementTangentialAcceleration readparam;
						readparam.AccelerationMinValue = reader.readFloat();	//�ݒ�����x�ŏ�
						readparam.AccelerationMaxValue = reader.readFloat();	//�ݒ�����x�ő�

						ParticleElementTangentialAcceleration *effectParam = new ParticleElementTangentialAcceleration();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Acceleration.setMinMax(readparam.AccelerationMinValue, readparam.AccelerationMaxValue);	//�ݒ�����x�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::InitColor:
					{
						//�J���[RGBA�F������
						EffectParticleElementInitColor readparam;
						readparam.ColorMinValue = reader.readU32();			//�ݒ�J���[�ŏ�
						readparam.ColorMaxValue = reader.readU32();			//�ݒ�J���[�ő�

						ParticleElementInitColor *effectParam = new ParticleElementInitColor();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��

						int a = (readparam.ColorMinValue & 0xFF000000) >> 24;
						int r = (readparam.ColorMinValue & 0x00FF0000) >> 16;
						int g = (readparam.ColorMinValue & 0x0000FF00) >> 8;
						int b = (readparam.ColorMinValue & 0x000000FF) >> 0;
						SsU8Color mincol(r, g, b, a);
						a = (readparam.ColorMaxValue & 0xFF000000) >> 24;
						r = (readparam.ColorMaxValue & 0x00FF0000) >> 16;
						g = (readparam.ColorMaxValue & 0x0000FF00) >> 8;
						b = (readparam.ColorMaxValue & 0x000000FF) >> 0;
						SsU8Color maxcol(r, g, b, a);
						effectParam->Color.setMinMax(mincol, maxcol);			//�ݒ�J���[�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TransColor:
					{
						//�J���[RGB�F�ω�
						EffectParticleElementTransColor readparam;
						readparam.ColorMinValue = reader.readU32();			//�ݒ�J���[�ŏ�
						readparam.ColorMaxValue = reader.readU32();			//�ݒ�J���[�ő�

						ParticleElementTransColor *effectParam = new ParticleElementTransColor();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��

						int a = (readparam.ColorMinValue & 0xFF000000) >> 24;
						int r = (readparam.ColorMinValue & 0x00FF0000) >> 16;
						int g = (readparam.ColorMinValue & 0x0000FF00) >> 8;
						int b = (readparam.ColorMinValue & 0x000000FF) >> 0;
						SsU8Color mincol(r, g, b, a);
						a = (readparam.ColorMaxValue & 0xFF000000) >> 24;
						r = (readparam.ColorMaxValue & 0x00FF0000) >> 16;
						g = (readparam.ColorMaxValue & 0x0000FF00) >> 8;
						b = (readparam.ColorMaxValue & 0x000000FF) >> 0;
						SsU8Color maxcol(r, g, b, a);
						effectParam->Color.setMinMax(mincol, maxcol);			//�ݒ�J���[�ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::AlphaFade:
					{
						//�t�F�[�h
						EffectParticleElementAlphaFade readparam;
						readparam.disprangeMinValue = reader.readFloat();		//�\����ԊJ�n
						readparam.disprangeMaxValue = reader.readFloat();		//�\����ԏI��

						ParticleElementAlphaFade *effectParam = new ParticleElementAlphaFade();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->disprange.setMinMax(readparam.disprangeMinValue, readparam.disprangeMaxValue);		//�\����ԊJ�n

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::Size:
					{
						//�X�P�[���F������
						EffectParticleElementSize readparam;
						readparam.SizeXMinValue = reader.readFloat();			//���{���ŏ�
						readparam.SizeXMaxValue = reader.readFloat();			//���{���ő�
						readparam.SizeYMinValue = reader.readFloat();			//�����{���ŏ�
						readparam.SizeYMaxValue = reader.readFloat();			//�����{���ő�
						readparam.ScaleFactorMinValue = reader.readFloat();		//�{���ŏ�
						readparam.ScaleFactorMaxValue = reader.readFloat();		//�{���ő�

						ParticleElementSize *effectParam = new ParticleElementSize();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->SizeX.setMinMax(readparam.SizeXMinValue, readparam.SizeXMaxValue);			//���{���ŏ�
						effectParam->SizeY.setMinMax(readparam.SizeYMinValue, readparam.SizeYMaxValue);			//�����{���ŏ�
						effectParam->ScaleFactor.setMinMax(readparam.ScaleFactorMinValue, readparam.ScaleFactorMaxValue);		//�{���ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TransSize:
					{
						//�X�P�[���F�ω�
						EffectParticleElementTransSize readparam;
						readparam.SizeXMinValue = reader.readFloat();			//���{���ŏ�
						readparam.SizeXMaxValue = reader.readFloat();			//���{���ő�
						readparam.SizeYMinValue = reader.readFloat();			//�����{���ŏ�
						readparam.SizeYMaxValue = reader.readFloat();			//�����{���ő�
						readparam.ScaleFactorMinValue = reader.readFloat();		//�{���ŏ�
						readparam.ScaleFactorMaxValue = reader.readFloat();		//�{���ő�

						ParticleElementTransSize *effectParam = new ParticleElementTransSize();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->SizeX.setMinMax(readparam.SizeXMinValue, readparam.SizeXMaxValue);			//���{���ŏ�
						effectParam->SizeY.setMinMax(readparam.SizeYMinValue, readparam.SizeYMaxValue);			//�����{���ŏ�
						effectParam->ScaleFactor.setMinMax(readparam.ScaleFactorMinValue, readparam.ScaleFactorMaxValue);		//�{���ŏ�

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::PointGravity:
					{
						//�d�͓_�̒ǉ�
						EffectParticlePointGravity readparam;
						readparam.Position_x = reader.readFloat();				//�d�͓_X
						readparam.Position_y = reader.readFloat();				//�d�͓_Y
						readparam.Power = reader.readFloat();					//�p���[

						ParticlePointGravity *effectParam = new ParticlePointGravity();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��
						effectParam->Position.x = readparam.Position_x;				//�d�͓_X
						effectParam->Position.y = readparam.Position_y;				//�d�͓_Y
						effectParam->Power = readparam.Power;					//�p���[

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::TurnToDirectionEnabled:
					{
						//�i�s�����Ɍ�����
						EffectParticleTurnToDirectionEnabled readparam;
						readparam.Rotation = reader.readFloat();					//�t���O

						ParticleTurnToDirectionEnabled *effectParam = new ParticleTurnToDirectionEnabled();
						effectParam->Rotation = readparam.Rotation;
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					case SsEffectFunctionType::InfiniteEmitEnabled:
					{
						EffectParticleInfiniteEmitEnabled readparam;
						readparam.flag = reader.readS32();					//�t���O

						ParticleInfiniteEmitEnabled *effectParam = new ParticleInfiniteEmitEnabled();
						effectParam->setType((SsEffectFunctionType::enum_)type);				//�R�}���h�̎��

						behavior.plist.push_back(effectParam);												//�p�����[�^��ǉ�
						break;
					}
					default:
					break;
					}
				}
				node->behavior = behavior;
				effectmodel->nodeList.push_back(node);
				if(nodeindex == 0)
				{
				}
			}
			//�c���[�̍\�z
			if(effectmodel->nodeList.size() > 0)
			{
				effectmodel->root = effectmodel->nodeList[0];	//root�m�[�h��ǉ�
				for(size_t i = 1; i < effectmodel->nodeList.size(); i++)
				{
					int pi = effectmodel->nodeList[i]->parentIndex;
					if(pi >= 0)
					{
						effectmodel->nodeList[pi]->addChildEnd(effectmodel->nodeList[i]);
					}
				}
			}
			effectmodel->lockRandSeed = effectFile->lockRandSeed; 	 // �����_���V�[�h�Œ�l
			effectmodel->isLockRandSeed = effectFile->isLockRandSeed;  // �����_���V�[�h���Œ肷�邩�ۂ�
			effectmodel->fps = effectFile->fps;             //
			effectmodel->effectName = effectFileName;
			effectmodel->layoutScaleX = effectFile->layoutScaleX;	//���C�A�E�g�X�P�[��X
			effectmodel->layoutScaleY = effectFile->layoutScaleY;	//���C�A�E�g�X�P�[��Y



			SSLOG("effect key: %s", effectFileName.c_str());
			_dic.insert(std::map<std::string, SsEffectModel*>::value_type(effectFileName, effectmodel));
		}
	}
	//�G�t�F�N�g�t�@�C�����̍폜
	void releseReference(void)
	{
		std::map<std::string, SsEffectModel*>::iterator it = _dic.begin();
		while(it != _dic.end())
		{
			SsEffectModel* effectmodel = it->second;

			if(effectmodel)
			{
				for(int nodeindex = 0; nodeindex < effectmodel->nodeList.size(); nodeindex++)
				{
					SsEffectNode* node = effectmodel->nodeList.at(nodeindex);
					for(int behaviorindex = 0; behaviorindex < node->behavior.plist.size(); behaviorindex++)
					{
						SsEffectElementBase* eb = node->behavior.plist.at(behaviorindex);
						delete eb;
					}
					node->behavior.plist.clear();
				}
				if(effectmodel->nodeList.size() > 0)
				{
					SsEffectNode* node = effectmodel->nodeList.at(0);
					delete node;
					effectmodel->nodeList.clear();
				}
				effectmodel->root = 0;

			}
			delete effectmodel;
			it++;
		}
		_dic.clear();
	}
protected:
	std::map<std::string, SsEffectModel*>		_dic;
};

} //namespace ss
