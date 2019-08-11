#pragma once

#include "hash.h"
#include <vector>

enum class MetaInfo : std::uint8_t{
  EMPTY,
  NOT_EMPTY
};

template <typename T_Key, typename T_Value>
class HashTable
{

private:
  static constexpr float load_factor{0.75f};
  static constexpr std::size_t default_capacity{2};
  std::size_t current_capacity{default_capacity};
  std::size_t item_count{0};

  struct KeyValueStorage{
    T_Key key{};
    T_Value value{};
  };
  

  std::vector<std::uint64_t> hash_storage{};
  std::vector<T_Key> key_storage{};
  std::vector<T_Value> value_storage{};
  std::vector<MetaInfo> meta_storage{};

public:
  std::size_t size() { return item_count; }
  std::size_t capacity() { return current_capacity; }

  HashTable()
  {
    hash_storage.resize(default_capacity);
    key_storage.resize(default_capacity);
    value_storage.resize(default_capacity);
    meta_storage.resize(default_capacity);
  }

  void IncreaseSize()
  {
    auto old_capacity = current_capacity;
    auto new_size = 2 * old_capacity;

    auto tmp_hash = hash_storage;
    auto tmp_key = key_storage;
    auto tmp_value = value_storage;
    auto tmp_meta = meta_storage;

    hash_storage.clear();
    hash_storage.resize(new_size);
    key_storage.clear();
    key_storage.resize(new_size);
    value_storage.clear();
    value_storage.resize(new_size);
    meta_storage.clear();
    meta_storage.resize(new_size);

    current_capacity = new_size;

    for (std::size_t i{0}; i < old_capacity; i++)
    {
      if (tmp_meta[i] != MetaInfo::EMPTY)
      {
        push(tmp_key[i], tmp_value[i]);
      }
    }
  }

  void push(const T_Key &key, const T_Value &value)
  {
    if (item_count > load_factor * current_capacity)
      IncreaseSize();

    const auto hash_val = Hash(key);
    auto index = hash_val % current_capacity;
    const auto start_index{index};

    while (meta_storage[index] != MetaInfo::EMPTY) // while there is already something
    {
      index++;

      if (index >= current_capacity)
        index = 0;
      if (index == start_index)
        IncreaseSize();
    }

    hash_storage[index] = hash_val;
    key_storage[index] = key;
    value_storage[index] = value;
    meta_storage[index] = MetaInfo::NOT_EMPTY;
    item_count++;
  }

  //std::vector<T_Value>::iterator find(const T_Key &key)
  std::vector<int>::iterator find(const T_Key &key)
  {
    const auto hash_val = Hash(key);
    auto index = hash_val % current_capacity;
    const auto start_index{index};

    while (meta_storage[index] != MetaInfo::EMPTY)
    {
      if (hash_storage[index] == hash_val)
        return std::next(value_storage.begin(), index);

      index++;

      if (index >= current_capacity)
        index = 0;
      if (index == start_index)
        break;
    }

    // nothing found
    return value_storage.end();
  }

  std::vector<int>::iterator end() { return value_storage.end(); }
};