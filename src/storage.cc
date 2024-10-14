#include <optional>
#include <sstream>
#include "storage.h"

Meta::~Meta() {

}

std::string Meta::toString(void) {
	std::ostringstream oss;
	
	oss << "{";
	for (const auto& pair: this->meta_info_) {
		oss << pair.first <<':'<< pair.second <<',';
	} oss << "}";

	return oss.str();
}

std::optional<meta_map> Meta::fromString(std::string ms) {
	const char MAP_START = '{';
	const char MAP_END = '}';
	const char MAP_KV_SEP = ':';
	const char MAP_SEP = ',';

	meta_map result;

	int ms_len = ms.length();
	int cur_index = 0;

	int key_start = -1;
	int value_start = -1;
	std::optional<std::string> key_word;
	std::optional<std::string> value_word;
	
	if (ms[ms_len-1]!=MAP_END) return std::nullopt;
	for (char ch: ms) {
		switch (ch) {
			case MAP_START:
				if (cur_index!=0) return std::nullopt;
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
				key_start = cur_index+1;
				break;
		} cur_index++;
		
		if (key_start>=0 && value_start>=0) {
			return std::nullopt;
		}
	}
	return result;
}
