
#include <iostream>

#include "gtest/gtest.h"

#include "hashtable.h"

TEST(HashTableSpec, return_size_0)
{
  HashTable<int, int> unit;

  EXPECT_EQ(unit.size(), 0);
}

TEST(HashTableSpec, return_size_1)
{
  HashTable<int, int> unit;

  unit.push(0, 0);

  EXPECT_EQ(unit.size(), 1);
}

TEST(HashTableSpec, return_size_2)
{
  HashTable<int, int> unit;

  unit.push(0, 0);
  unit.push(1, 1);

  EXPECT_EQ(unit.size(), 2);
}

TEST(HashTableSpec, find_0)
{
  HashTable<int, int> unit;

  auto key{0};
  auto value{0};

  unit.push(key, value);

  EXPECT_EQ(*unit.find(key), value);
}

TEST(HashTableSpec, find_1)
{
  HashTable<int, int> unit;

  auto key{1};
  auto value{1};

  unit.push(key, value);

  EXPECT_EQ(*unit.find(key), value);
}

TEST(HashTableSpec, find_multiple)
{
  HashTable<int, int> unit;

  std::vector<std::pair<int, int>> vec{{0, 0}, {1, 1}};

  for (auto pair : vec)
  {
    unit.push(pair.first, pair.second);
  }

  for (auto pair : vec)
  {
    EXPECT_EQ(*unit.find(pair.first), pair.second);
  }
}

TEST(HashTableSpec, find_multiple_colission)
{
  HashTable<int, int> unit;

  std::vector<std::pair<int, int>> vec{{1, 1}, {3, 3}};

  //std::cout << "pushing pairs" << std::endl;
  for (auto pair : vec)
  {
    unit.push(pair.first, pair.second);
    //std::cout << unit.find(pair.first) << " : " << pair.second;
    //std::cout << " with index: " << Hash(pair.first) % 4 << std::endl;
  }

  //std::cout << "finding pairs" << std::endl;
  for (auto pair : vec)
  {
    //std::cout << unit.find(pair.first) << " : " << pair.second;
    //std::cout << " with index: " << Hash(pair.first) % 4 << std::endl;

    EXPECT_EQ(*unit.find(pair.first), pair.second);
  }
}

TEST(HashTableSpec, increase_size_if_full)
{
  HashTable<int, int> unit;
  const auto start_capacity{unit.capacity()};

  for (int i = 0; i < 100; i++)
  {
    unit.push(i,i);
  }

  EXPECT_GT(unit.capacity(), start_capacity);
}

TEST(HashTableSpec, find_if_size_increased)
{
  HashTable<int, int> unit;

  for (int i = 0; i < 10000; i++)
  {
    unit.push(i,i);
  }

  for (int i = 0; i < 10000; i++)
  {
    EXPECT_EQ(*unit.find(i), i);
  }
}

TEST(HashTableSpec, find_non_existing)
{
  HashTable<int, int> unit;

  EXPECT_EQ(unit.find(0), unit.end());
  
}



int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "HashTableSpec";
  return RUN_ALL_TESTS();
}