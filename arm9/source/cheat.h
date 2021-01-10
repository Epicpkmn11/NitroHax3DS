
#ifndef CHEAT_H
#define CHEAT_H

#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>

#define CHEAT_CODE_END	0xCF000000
#define CHEAT_ENGINE_RELOCATE	0xCF000001
#define CHEAT_ENGINE_HOOK	0xCF000002


typedef unsigned int CheatWord;

class CheatFolder;

class CheatBase
{
private:
	CheatFolder* parent;

public:
	std::string name;
	std::string note;

	CheatBase (CheatFolder* parent)
	{
		this->parent = parent;
	}

	CheatBase (const char* name, CheatFolder* parent)
	{
		this->name = name;
		this->parent = parent;
	}

	CheatBase (const std::string name, CheatFolder* parent)
	{
		this->name = name;
		this->parent = parent;
	}

	virtual ~CheatBase ()
	{
	}

	const char* getName (void)
	{
		return name.c_str();
	}

	const char* getNote (void)
	{
		return note.c_str();
	}

	CheatFolder* getParent (void)
	{
		return parent;
	}

	virtual std::vector<CheatWord> getEnabledCodeData(void)
	{
		std::vector<CheatWord> codeData;
		return codeData;
	}
} ;

class CheatCode : public CheatBase
{
public:
	CheatCode (CheatFolder* parent) : CheatBase (parent)
	{
		enabled = false;
		always_on = false;
		master = false;
	}

	void setEnabled (bool enable)
	{
		if (!always_on) {
			enabled = enable;
		}
	}

	void toggleEnabled (void);

	bool getEnabledStatus (void)
	{
		return enabled;
	}

	bool isMaster (void)
	{
		return master;
	}

	std::vector<CheatWord> getCodeData(void)
	{
		return cheatData;
	}

	void setCodeData (const CheatWord *codeData, int codeLen);

	std::vector<CheatWord> getEnabledCodeData(void);

	static const int CODE_WORD_LEN = 8;

private:
	std::vector<CheatWord> cheatData;
	bool enabled;
	bool always_on;
	bool master;
} ;

class CheatFolder : public CheatBase
{
public:
	CheatFolder (const char* name, CheatFolder* parent) : CheatBase (name, parent)
	{
		allowOneOnly = false;
	}

	CheatFolder (CheatFolder* parent) : CheatBase (parent)
	{
		allowOneOnly = false;
	}

	~CheatFolder();

	void addItem (CheatBase* item)
	{
		if (item) {
			contents.push_back(item);
		}
	}

	void enablingSubCode (void);

	void enableAll (bool enabled);

	void setAllowOneOnly (bool value)
	{
		allowOneOnly = value;
	}

	std::vector<CheatBase*> getContents(void)
	{
		return contents;
	}

	std::vector<CheatWord> getEnabledCodeData(void);

protected:
	std::vector<CheatBase*> contents;

private:
	bool allowOneOnly;

} ;

class CheatGame : public CheatFolder
{
public:
	CheatGame (const char* name, CheatFolder* parent) : CheatFolder (name, parent)
	{
		gameid = 0x20202020;
	}

	CheatGame (CheatFolder* parent) : CheatFolder (parent)
	{
		gameid = 0x20202020;
	}

	bool checkGameid (uint32_t gameid, uint32_t headerCRC)
	{
		return (gameid == this->gameid) && (headerCRC == this->headerCRC);
	}

	void setGameid (uint32_t id, uint32_t crc);

private:
	uint32_t gameid;
	uint32_t headerCRC;
} ;

class CheatCodelist : public CheatFolder
{
public:
	CheatCodelist (void) : CheatFolder ("No codes loaded", NULL)
	{
	}

	~CheatCodelist ();

	bool load (FILE* fp, uint32_t gameid, uint32_t headerCRC, bool filter);

	CheatGame* getGame (uint32_t gameid, uint32_t headerCRC);

private:
	struct sDatIndex
	{
		uint32_t _gameCode;
		uint32_t _crc32;
		uint64_t _offset;
	};
	bool searchCheatData(FILE* aDat, uint32_t gamecode, uint32_t crc32, long& aPos, size_t& aSize);

} ;

#endif // CHEAT_H
