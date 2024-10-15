#include <optional>
#include <sstream>
#include "storage.h"

std::string Meta::toString(void) {
	std::ostringstream oss;
	
	oss << MAP_START;
	for (const auto& pair: this->meta_info_) {
		oss << pair.first <<MAP_KV_SEP<< pair.second <<MAP_SEP;
	} oss << MAP_END;

	return oss.str();
}


std::optional<meta_map> Meta::fromString(std::string ms) {
	meta_map result;

	int ms_len = ms.length();
	int cur_index = 0; // 遍历中当前索引

	int key_start = -1; // 键开始的索引
	int value_start = -1; // 键结束的索引
	std::optional<std::string> key_word;
	std::optional<std::string> value_word;
	
	if (ms[ms_len-1]!=MAP_END || ms[0]!=MAP_START) return std::nullopt;
	// 检查开始和结束的符号是否正确
	if (ms_len==2) return result;
	// 检查 `{}` 的情形
	for (char ch: ms) {
		switch (ch) {
			case MAP_START:
				if (cur_index!=0) return std::nullopt;
				// 检查开始的符号是否唯一  
				key_start = 1; 	
				break;
			case MAP_KV_SEP:
				if (key_start<1) return std::nullopt;
				// 去除键为空的情形

				key_word = ms.substr(key_start, cur_index-key_start);
				key_start = -1;
				// 结束 key_word 的读取 
				
				if (cur_index+1>=ms_len) return std::nullopt;
				// 去除以 `MAP_KV_SEP` 结尾的情形
				value_start = cur_index+1;
				// 开始 value 的读取
				break;
			case MAP_END:
				if (cur_index!=ms_len-1) return std::nullopt;
				// 检查结束的符号是否唯一

				if (value_start<0&&result.size()==0) return std::nullopt;
				// 去除没有 `MAP_SEP` 的情形,比如 `{ABC}`
				if (key_word.has_value()&&value_start<0) return std::nullopt;
				if (!key_word.has_value()&&key_start>0) { 
					if (key_start==cur_index) {
						break;
					} else {
						return std::nullopt;
					} 
				} // 去除连续 `MAP_SEP` 的情形,不包括多余逗号的情形

				value_word = ms.substr(value_start, cur_index-value_start);
				result.insert(std::make_pair(key_word.value(), value_word.value()));
				// 将读取的键值对写入字典中

				break;
			case MAP_SEP:
				if (!key_word.has_value()&&value_word.has_value()) return std::nullopt;
				if (value_start<1) return std::nullopt;
				
				value_word = ms.substr(value_start, cur_index-value_start);
				result.insert(std::make_pair(key_word.value(), value_word.value()));
				// 将读取的键值对写入字典中
				key_word = std::nullopt;
				value_word = std::nullopt;
				// 重置键值

				value_start = -1;
				if (cur_index+1>=ms_len) return std::nullopt;
				// 去除以 `MAP_SEP` 结尾的情形
				key_start = cur_index+1;
				break;
		} cur_index++;
		
		if (key_start>=0 && value_start>=0)
			return std::nullopt; // 去除连续 `MAP_KV_SEP` 的情形
	}
	return result;
}

std::optional<std::string> Meta::operator[](std::string key) {
    try {
        return this->meta_info_.at(key);
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }	
}

bool operator==(const Meta& lhs, const Meta& rhs) {
	return lhs.meta_info_==rhs.meta_info_;
};
