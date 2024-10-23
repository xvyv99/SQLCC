#ifndef _DATAFRAME_HPP
#define _DATAFRAME_HPP

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>
#include <unordered_set>

using df_shape = std::pair<std::size_t, std::size_t>;
using index_name = std::vector<std::string>;

template <typename T>
class DataFrame {
private:
    std::vector<std::vector<T>> data_;
    index_name col_names_;
    df_shape shape_;

    std::optional<std::size_t> canFindName(std::string_view);
public:
    DataFrame(void);
    DataFrame(index_name);
    ~DataFrame(void);

    df_shape shape(void);
    bool hasValue(void);
    bool setColN(index_name);

    bool addRow(std::vector<T>);
    // void addCol(std::vector<T> col);

    std::optional<T> accessItem(std::size_t, std::string_view);
    std::optional<T> accessItem(std::size_t, std::size_t);
    std::optional<std::vector<T>> accessCol(std::string_view);
    std::optional<std::vector<T>> accessRow(std::size_t);

    std::optional<T> operator()(std::size_t, std::string_view);
    std::optional<T> operator()(std::size_t, std::size_t);

    std::optional<std::vector<T>> operator[](std::size_t);
    std::optional<std::vector<T>> operator[](std::string_view);
};

template <typename T>
DataFrame<T>::DataFrame() {
    this->data_ = std::vector<std::vector<T>>();
    this->shape_.first = 0;
    this->shape_.second = 0;
}

template <typename T>
DataFrame<T>::DataFrame(index_name coln) {
    this->data_ = std::vector<std::vector<T>>();
    this->shape_.first = 0;
    this->shape_.second = coln.size();
    this->col_names_ = coln;
}

template <typename T>
std::optional<std::size_t> DataFrame<T>::canFindName(std::string_view col_name) {
    auto it = std::find(this->col_names_.begin(), this->col_names_.end(), col_name);
    if (it!=this->col_names_.end()) {
        return std::distance(this->col_names_.begin(), it);
    } else {
        return std::nullopt;
    }
}

template <typename T>
DataFrame<T>::~DataFrame() {}

template <typename T>
df_shape DataFrame<T>::shape() {
    return this->shape_;
}

template <typename T>
bool DataFrame<T>::hasValue() {
    return this->data_.size()!=0;
}

template <typename T>
bool DataFrame<T>::setColN(index_name coln) {
    if ((this->shape_.second!=coln.size() && this->shape_.second>0) || coln.size()==0)
        return false;
     
    std::unordered_set<std::string> tmp_set(coln.begin(), coln.end());
    if (tmp_set.size()!=coln.size())
        return false;

    if (this->shape_.second==0)
        this->shape_.second = coln.size();
    this->col_names_ = coln;
    return true;
}

template <typename T>
bool DataFrame<T>::addRow(std::vector<T> row) {
    if (row.size()!=this->shape_.second) {
        return false;
    } else {
        this->data_.push_back(row);
        this->shape_.first += 1;
        return true;
    }
}

template <typename T>
std::optional<T> DataFrame<T>::accessItem(std::size_t row_num, std::string_view col_name) {
    if (row_num>=this->shape().first) 
        return std::nullopt;
    
    std::optional<std::size_t> res_o = this->canFindName(col_name);
    if (res_o.has_value()) {
        return this->data_[row_num][res_o.value()];
    } else {
        return std::nullopt;
    }
}

template <typename T>
std::optional<T> DataFrame<T>::accessItem(std::size_t row_num, std::size_t col_num) {
    if (row_num<this->shape().first && col_num<this->shape().second) {
        return this->data_[row_num][col_num];
    } else {
        return std::nullopt;
    }
}

template <typename T>
std::optional<std::vector<T>> DataFrame<T>::accessCol(std::string_view col_name) {
    std::optional<std::size_t> res_o = this->canFindName(col_name);
    if (!res_o.has_value()) {
        return std::nullopt;
    } else {
        std::size_t col_num = res_o.value();
        std::size_t line_count = this->shape().first;
        std::vector<T> res_vec; res_vec.reserve(line_count);
        for (int i=0;i<line_count;i++)
            res_vec.push_back(this->data_[i][col_num]);
        return res_vec;
    }
};

template <typename T>
std::optional<std::vector<T>> DataFrame<T>::accessRow(std::size_t row_num) {
    if (row_num>=this->shape().first) {
        return std::nullopt;
    } else {
        return this->data_[row_num];
    }
};

template <typename T>
std::optional<T> DataFrame<T>::operator()(std::size_t row_num, std::string_view col_name) {
    return this->accessItem(row_num, col_name);
};

template <typename T>
std::optional<T> DataFrame<T>::operator()(std::size_t row_num, std::size_t col_num) {
    return this->accessItem(row_num, col_num);
};

template <typename T>
std::optional<std::vector<T>> DataFrame<T>::operator[](std::size_t row_num) {
    return this->accessRow(row_num);
};

template <typename T>
std::optional<std::vector<T>> DataFrame<T>::operator[](std::string_view col_name) {
    return this->accessCol(col_name);
};

#endif