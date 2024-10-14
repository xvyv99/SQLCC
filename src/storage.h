#ifndef STORAGE_H
#define STORAGE_H

#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class Meta {
protected:
	std::unordered_map<std::string, std::string> meta_info_;
public:
	Meta();
	~Meta();

	std::string toString(void);
};

class BaseDB {
public:
	BaseDB();
	~BaseDB();

	virtual bool addItem(fs::path, Meta);
	virtual bool changeItem(fs::path, Meta);
	virtual bool delItem(fs::path, Meta);
};

class FileName: public BaseDB {
public:
	FileName();
	~FileName();
};

class SQLite: public BaseDB {
public:
	SQLite();
	~SQLite();
};

#endif
