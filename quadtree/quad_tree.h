//
// Created by fberchtold on 9/25/19.
// TODO store pointers to a continous array?! possible, since its <typename T>
//

#ifndef AUTOMATA_QUAD_TREE_H
#define AUTOMATA_QUAD_TREE_H

#include <vector>

#include "lib/Vector2D.h"

using Point = Vector2D<int>;

template <typename T>
struct Node {
  Node(T _value, Point _point) : value(_value), point(_point) {
  }
  T value;
  Point point;
};

template <typename T>
class QuadTree {
 public:
  void AddNode(T value, Point pos) {
    if (abs(m_top_left.x - m_bot_right.x) <= 1 && abs(m_top_left.y - m_bot_right.y) <= 1) {
      m_nodes.emplace_back(value, pos);
      return;
    }

    const auto has_children = m_child_sw || m_child_nw || m_child_se || m_child_ne;
    if (has_children || m_nodes.size() >= m_bucket_size) {
      // bucket full, new node needed
      const auto center_x = (m_top_left.x + m_bot_right.x) / 2;
      const auto center_y = (m_top_left.y + m_bot_right.y) / 2;

      if (pos.x < center_x) {
        // west
        if (pos.y < center_y) {
          // sw
          if (!m_child_sw) {
            m_child_sw = std::make_unique<QuadTree<T>>();
            m_child_sw->m_top_left = {m_top_left.x, center_y};
            m_child_sw->m_bot_right = {center_x, m_bot_right.y};
          }
          m_child_sw->AddNode(value, pos);
        } else {
          // nw
          if (!m_child_nw) {
            m_child_nw = std::make_unique<QuadTree<T>>();
            m_child_nw->m_top_left = m_top_left;
            m_child_nw->m_bot_right = {center_x, center_y};
          }
          m_child_nw->AddNode(value, pos);
        }
      } else {
        // east
        if (pos.y < center_y) {
          // se
          if (!m_child_se) {
            m_child_se = std::make_unique<QuadTree<T>>();
            m_child_se->m_top_left = {center_x, center_y};
            m_child_se->m_bot_right = m_bot_right;
          }
          m_child_se->AddNode(value, pos);
        } else {
          // ne
          if (!m_child_ne) {
            m_child_ne = std::make_unique<QuadTree<T>>();
            m_child_ne->m_top_left = {center_x, m_top_left.y};
            m_child_ne->m_bot_right = {m_bot_right.x, center_y};
          }
          m_child_ne->AddNode(value, pos);
        }
      }

      // now we need to put all currently stored items into lower nodes
      auto temp_nodes = m_nodes;
      m_nodes.clear();
      for (auto node : temp_nodes) {
        AddNode(node.value, node.point);
      }
      m_nodes.clear();
    } else {
      m_nodes.emplace_back(value, pos);
    }
  }

  int Size() {
    int size{};

    size += static_cast<int>(m_nodes.size());

    size += m_child_nw ? m_child_nw->Size() : 0;
    size += m_child_ne ? m_child_ne->Size() : 0;
    size += m_child_sw ? m_child_sw->Size() : 0;
    size += m_child_se ? m_child_se->Size() : 0;

    return size;
  }

  std::vector<T> Search(Point search_area_top_left, Point search_area_bot_right) {
    std::vector<T> return_vector;

    const bool is_x_within_node_area =
        !(m_top_left.x > search_area_bot_right.x || m_bot_right.x < search_area_top_left.x);

    const bool is_y_within_node_area =
        !(m_top_left.y < search_area_bot_right.y || m_bot_right.y > search_area_top_left.y);

    if (is_x_within_node_area && is_y_within_node_area) {
      for (auto node : m_nodes) {
        return_vector.push_back(node.value);
      }

      if (m_child_nw) {
        auto child_values = m_child_nw->Search(search_area_top_left, search_area_bot_right);
        return_vector.insert(return_vector.end(), std::begin(child_values), std::end(child_values));
      }
      if (m_child_ne) {
        auto child_values = m_child_ne->Search(search_area_top_left, search_area_bot_right);
        return_vector.insert(return_vector.end(), std::begin(child_values), std::end(child_values));
      }
      if (m_child_sw) {
        auto child_values = m_child_sw->Search(search_area_top_left, search_area_bot_right);
        return_vector.insert(return_vector.end(), std::begin(child_values), std::end(child_values));
      }
      if (m_child_se) {
        auto child_values = m_child_se->Search(search_area_top_left, search_area_bot_right);
        return_vector.insert(return_vector.end(), std::begin(child_values), std::end(child_values));
      }
    }

    return return_vector;
  }

  void SetPoints(Point top_left, Point bot_right) {
    m_top_left = top_left;
    m_bot_right = bot_right;
  }

 private:
  size_t m_bucket_size{1};

  std::vector<Node<T>> m_nodes;

 public:
  Point m_top_left;
  Point m_bot_right;

  std::unique_ptr<QuadTree<T>> m_child_nw;
  std::unique_ptr<QuadTree<T>> m_child_ne;
  std::unique_ptr<QuadTree<T>> m_child_sw;
  std::unique_ptr<QuadTree<T>> m_child_se;
};

// template <typename T>
// class QuadTree {
// public:
//  QuadTree(Point top_left, Point bot_right)
//  {
//    m_root.SetPoints(top_left, bot_right);
//  }
//
//
//  void AddNode(T value, Point pos) {
//    m_root.AddNode(value, pos);
//  }
//
//  int Size() {
//    return m_root.Size();
//  }
//
//  std::vector<T> Search(Point left_top, Point right_bot) {
//    return m_root.Search(left_top, right_bot);
//  }
//
// private:
//  Node<T> m_root;
//};

#endif  // AUTOMATA_QUAD_TREE_H
