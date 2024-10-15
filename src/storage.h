#ifndef STORAGE_H
#define STORAGE_H

#include <optional>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;
using meta_map = std::unordered_map<std::string, std::string>;

class Meta {
protected:
	meta_map meta_info_;
public:
	const static char MAP_START = '{';
	const static char MAP_END = '}';
	const static char MAP_KV_SEP = ':'; // 键与值的分隔符
	const static char MAP_SEP = ',';

	Meta(meta_map mm) { this->meta_info_=mm; };
	~Meta() {};

	std::string toString(void);
	static std::optional<meta_map> fromString(std::string);

	std::optional<std::string> operator[](std::string);
	bool friend operator==(const Meta&, const Meta&);
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
