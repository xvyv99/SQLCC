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

	int key_start = -1;
	int value_start = -1;
	std::optional<std::string> key_word;
	std::optional<std::string> value_word;
	
	if (ms[ms_len-1]!=MAP_END) return std::nullopt;
	// 检查结束的符号是否正确
	for (char ch: ms) {
		switch (ch) {
			case MAP_START:
				if (cur_index!=0) return std::nullopt;
				// 检查开始的符号是否正确且唯一,去除 `{A{B}` 和 `A}`  
				key_start = 1; 	
				break;
			case MAP_KV_SEP:
				if (value_word.has_value()&&key_word.has_value()) return std::nullopt;
				if (key_start<1) return std::nullopt;
				key_word = ms.substr(key_start, cur_index-key_start);
				key_start = -1;
				if (cur_index+1>=ms_len) return std::nullopt;
				value_start = cur_index+1;
				break;
			case MAP_END:
				if (cur_index!=ms_len-1) return std::nullopt;
				// 检查结束的符号是否唯一,去除 `{A}B}`
				if (!key_word.has_value()&&value_word.has_value()) {
					return std::nullopt;
				} else if ((!key_word.has_value())&&(!value_word.has_value())) {
					break;
				}

				if (value_start<1) return std::nullopt;
				
				value_word = ms.substr(value_start, cur_index-value_start);
				result.insert(std::make_pair(key_word.value(), value_word.value()));

				break;
			case MAP_SEP:
				if (!key_word.has_value()&&value_word.has_value()) return std::nullopt;
				if (value_start<1) return std::nullopt;
				
				value_word = ms.substr(value_start, cur_index-value_start);
				result.insert(std::make_pair(key_word.value(), value_word.value()));
				key_word = std::nullopt;
				value_word = std::nullopt;

				value_start = -1;
				if (cur_index+1>=ms_len) return std::nullopt;
				// 去除
				key_start = cur_index+1;
				break;
		} cur_index++;
		
		if (key_start>=0 && value_start>=0)
			return std::nullopt; // 去除 '{A,}'
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
