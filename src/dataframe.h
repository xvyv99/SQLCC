#ifndef _DATAFRAME_H
#define _DATAFRAME_H

#include <string>
#include <vector>

template <typename T>
class DataFrame {
private:
    std::vector<std::vector<T>> data_;
    std::vector<std::string> col_name_;
public:
    DataFrame(/* args */);
    ~DataFrame();
};

#endif