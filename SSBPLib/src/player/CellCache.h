#pragma once

namespace ss{

/**
 * CellRef
 */
struct CellRef
{
	const Cell* cell;
	TextuerData texture;
	SSRect rect;
	std::string texname;
};


/**
 * CellCache
 */
class CellCache
{
public:
	CellCache()
	{
	}
	~CellCache()
	{
		releseReference();
	}

	static CellCache* create(const ProjectData* data, const std::string& imageBaseDir)
	{
		CellCache* obj = new CellCache();
		if(obj)
		{
			obj->init(data, imageBaseDir);
		}
		return obj;
	}

	CellRef* getReference(int index)
	{
		if(index < 0 || index >= (int)_refs.size())
		{
			SSLOGERROR("Index out of range > %d", index);
			SS_ASSERT(0);
		}
		CellRef* ref = _refs.at(index);
		return ref;
	}

	//�w�肵�����O�̃Z���̎Q�ƃe�N�X�`����ύX����
	bool setCellRefTexture(const ProjectData* data, const char* cellName, long texture)
	{
		bool rc = false;

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));

		//���O����C���f�b�N�X�̎擾
		int cellindex = -1;
		for(int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));
			const char* name = static_cast<const char*>(ptr(cellMap->name));
			if(strcmp(cellName, name) == 0)
			{
				CellRef* ref = getReference(i);
				ref->texture.handle = texture;
				rc = true;
			}
		}

		return(rc);
	}

	//�w�肵���f�[�^�̃e�N�X�`����j������
	bool releseTexture(const ProjectData* data)
	{
		bool rc = false;

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));
		for(int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));
			{
				CellRef* ref = _refs.at(i);
				if(ref->texture.handle != -1)
				{
					SSTextureRelese(ref->texture.handle);
					ref->texture.handle = -1;
					rc = true;
				}
			}
		}
		return(rc);
	}

protected:
	void init(const ProjectData* data, const std::string& imageBaseDir)
	{

		SS_ASSERT2(data != NULL, "Invalid data");

		_textures.clear();
		_refs.clear();
		_texname.clear();

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));

		for(int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));

			if(cellMap->index >= (int)_textures.size())
			{
				const char* imagePath = static_cast<const char*>(ptr(cellMap->imagePath));
				addTexture(imagePath, imageBaseDir, (SsTexWrapMode::_enum)cellMap->wrapmode, (SsTexFilterMode::_enum)cellMap->filtermode);
			}

			//�Z����񂾂�����Ă���
			//�e�N�X�`���̓ǂݍ��݂̓Q�[�����ɔC����
			CellRef* ref = new CellRef();
			ref->cell = cell;
			ref->texture = _textures.at(cellMap->index);
			ref->texname = _texname.at(cellMap->index);
			ref->rect = SSRect(cell->x, cell->y, cell->width, cell->height);
			_refs.push_back(ref);
		}

	}
	//�L���b�V���̍폜
	void releseReference(void)
	{
		for(int i = 0; i < _refs.size(); i++)
		{
			CellRef* ref = _refs.at(i);
			if(ref->texture.handle != -1)
			{
				SSTextureRelese(ref->texture.handle);
				ref->texture.handle = -1;
			}
			delete ref;
		}
		_refs.clear();
	}

	void addTexture(const std::string& imagePath, const std::string& imageBaseDir, SsTexWrapMode::_enum  wrapmode, SsTexFilterMode::_enum filtermode)
	{
		std::string path = "";

		if(isAbsolutePath(imagePath))
		{
			// ��΃p�X�̂Ƃ��͂��̂܂܈���
			path = imagePath;
		}
		else
		{
			// ���΃p�X�̂Ƃ���imageBaseDir��t�^����
			path.append(imageBaseDir);
			size_t pathLen = path.length();
			if(pathLen && path.at(pathLen - 1) != '/' && path.at(pathLen - 1) != '\\')
			{
				path.append("/");
			}
			path.append(imagePath);
		}

		//�e�N�X�`���̓ǂݍ���
		long tex = SSTextureLoad(path.c_str(), wrapmode, filtermode);
		SSLOG("load: %s", path.c_str());
		TextuerData texdata;
		texdata.handle = tex;
		int w;
		int h;
		SSGetTextureSize(texdata.handle, w, h);
		texdata.size_w = w;
		texdata.size_h = h;

		_textures.push_back(texdata);
		_texname.push_back(path);

	}

protected:
	std::vector<std::string>			_texname;
	std::vector<TextuerData>			_textures;
	std::vector<CellRef*>				_refs;
};

} //namespace ss
