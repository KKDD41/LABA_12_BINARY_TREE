#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <initializer_list>
#include <iterator>
#include <vector>
#include <queue>

template<typename T>
class BinarySearchTree {
 private:
  struct TreeNode {
    template<typename U>
    explicit TreeNode(U&& value) : value_(std::forward<U>(value)) {}
    template<typename... Args>
    explicit TreeNode(Args&& ... args) : value_(std::forward<Args>(args)...) {}

    bool IsLeaf() const { return left_ == nullptr && right_ == nullptr; }

    T value_;
    TreeNode* left_ = nullptr;
    TreeNode* right_ = nullptr;
    TreeNode* parent_ = nullptr;
  };
  TreeNode* root_ = nullptr;
  int size_ = 0;

 public:
  class ConstIterator :
      public std::iterator<
          std::bidirectional_iterator_tag, T, std::ptrdiff_t, T*, const T&> {
   public:
    friend BinarySearchTree<T>;

    // CONSTRUCTORS & DESTRUCTORS
    ConstIterator(const ConstIterator& iter) = default;
    ~ConstIterator() = default;

    ConstIterator& operator=(const ConstIterator& iter) = default;

    // DO SOMETHING
    const T& operator*() const { return current_node_->value_; }
    const T* operator->() const { return &current_node_->value_; }

    // UNARY OPERATORS
    ConstIterator& operator++();
    ConstIterator operator++(int);
    ConstIterator& operator--();
    ConstIterator operator--(int);

    // COMPARISON OPERATORS
    bool operator==(const ConstIterator& rhs) const;
    bool operator!=(const ConstIterator& rhs) const;

   private:
    ConstIterator(BinarySearchTree<T>* tree, TreeNode* node) :
        tree_(tree), current_node_(node) {}

    TreeNode* Next();
    TreeNode* Previous();

    BinarySearchTree<T>* tree_;
    TreeNode* current_node_;
  };

  // CONSTRUCTORS
  BinarySearchTree() = default;
  BinarySearchTree(std::initializer_list<T> list);
  BinarySearchTree(const BinarySearchTree<T>& tree);
  BinarySearchTree(BinarySearchTree<T>&& tree) noexcept;

  // DELETING
  ~BinarySearchTree();
  void erase(ConstIterator iterator);
  void erase(const T& value);

  // ASSIGNMENT OPERATORS
  BinarySearchTree& operator=(const BinarySearchTree<T>& rhs);
  BinarySearchTree& operator=(BinarySearchTree<T>&& rhs) noexcept;

  // GETTERS
  int size() const { return size_; }
  bool empty() const { return size_ == 0; }
  bool contains(const T& value) const { return end() != find(value); }
  int count(const T& value) const;

  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator find(const T& value) const;
  std::vector<T> to_vector() const;

  // TREE CHANGING
  template<typename U>
  void insert(U&& value);
  template<typename... Args>
  void emplace(Args&& ... args);

  // COMPARISON OPERATORS
  bool operator==(const BinarySearchTree<T>& rhs) const;
  bool operator!=(const BinarySearchTree<T>& rhs) const;

 private:
  void DeleteSubtree(TreeNode* node);
  void DeleteLeaf(TreeNode* leaf_to_erase);
  void DeleteNodeWithOneChild(TreeNode* node);

  // Relation 0 means that 'first_parent == nullptr' and 'first' must be a root.
  // Relation -1 or 1 means that 'first' is the left or the right child of the
  // 'first_parent'
  void CopySubtree(TreeNode* first_parent, TreeNode* first,
                   TreeNode* second, int relation = 0);
  void InsertNode(TreeNode* node);
};

// CONSTRUCTORS
template<typename T>
BinarySearchTree<T>::BinarySearchTree(const BinarySearchTree<T>& tree) {
  (*this) = tree;
}
template<typename T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTree<T>&& tree) noexcept {
  (*this) = std::move(tree);
}
template<typename T>
BinarySearchTree<T>::BinarySearchTree(std::initializer_list<T> list) {
  for (const T& element : list) {
    this->insert(element);
  }
}

// ASSIGNMENT OPERATORS
template<typename T>
BinarySearchTree<T>&
BinarySearchTree<T>::operator=(const BinarySearchTree<T>& rhs) {
  if (this != &rhs) {
    this->DeleteSubtree(root_);
    this->CopySubtree(nullptr, root_, rhs.root_);
    size_ = rhs.size_;
  }
  return *this;
}
template<typename T>
BinarySearchTree<T>& BinarySearchTree<T>::operator=(BinarySearchTree<T>&& rhs)
                                                                    noexcept {
  if (this != &rhs) {
    this->DeleteSubtree(root_);
    this->CopySubtree(nullptr, root_, rhs.root_);
    size_ = rhs.size_;
    rhs.DeleteSubtree(rhs.root_);
  }
  return *this;
}

template<typename T>
void BinarySearchTree<T>::CopySubtree(TreeNode* first_parent,
                                      TreeNode* first, TreeNode* second,
                                      int relation) {
  if (second == nullptr) {
    return;
  }
  first = new TreeNode(second->value_);
  first->parent_ = first_parent;
  if (relation == 0) {
    root_ = first;
  } else if (relation == -1) {
    first_parent->left_ = first;
  } else {
    first_parent->right_ = first;
  }
  this->CopySubtree(first, first->left_, second->left_, -1);
  this->CopySubtree(first, first->right_, second->right_, 1);
}

// DELETING
template<typename T>
BinarySearchTree<T>::~BinarySearchTree() {
  this->DeleteSubtree(root_);
}

template<typename T>
void BinarySearchTree<T>::DeleteLeaf(BinarySearchTree::TreeNode*
leaf_to_erase) {
  if (leaf_to_erase == nullptr) {
    return;
  }
  if (leaf_to_erase->parent_ != nullptr) {
    if (leaf_to_erase->parent_->left_ == leaf_to_erase) {
      leaf_to_erase->parent_->left_ = nullptr;
    } else {
      leaf_to_erase->parent_->right_ = nullptr;
    }
  } else {
    root_ = nullptr;
  }
  delete leaf_to_erase;
  size_--;
}

template<typename T>
void BinarySearchTree<T>::DeleteSubtree(BinarySearchTree::TreeNode* node) {
  if (node == nullptr) {
    return;
  }
  this->DeleteSubtree(node->left_);
  this->DeleteSubtree(node->right_);
  this->DeleteLeaf(node);
}

template<typename T>
void BinarySearchTree<T>::DeleteNodeWithOneChild(TreeNode* node_to_erase) {
  if (node_to_erase->right_ == nullptr) {
    node_to_erase->left_->parent_ = node_to_erase->parent_;
    if (node_to_erase->parent_ == nullptr) {
      root_ = node_to_erase->left_;
    } else {
      if (node_to_erase->parent_->left_ == node_to_erase) {
        node_to_erase->parent_->left_ = node_to_erase->left_;
      } else {
        node_to_erase->parent_->right_ = node_to_erase->left_;
      }
    }
  } else if (node_to_erase->left_ == nullptr) {
    node_to_erase->right_->parent_ = node_to_erase->parent_;
    if (node_to_erase->parent_ == nullptr) {
      root_ = node_to_erase->right_;
    } else {
      if (node_to_erase->parent_->left_ == node_to_erase) {
        node_to_erase->parent_->left_ = node_to_erase->right_;
      } else {
        node_to_erase->parent_->right_ = node_to_erase->right_;
      }
    }
  }
  delete node_to_erase;
  size_--;
}

template<typename T>
void BinarySearchTree<T>::erase(BinarySearchTree::ConstIterator iterator) {
  TreeNode* node_to_erase = iterator.current_node_;
  if (node_to_erase == nullptr) {
    return;
  }
  if (node_to_erase->IsLeaf()) {
    this->DeleteLeaf(node_to_erase);
  } else if (node_to_erase->right_ == nullptr ||
      node_to_erase->left_ == nullptr) {
    this->DeleteNodeWithOneChild(node_to_erase);
  } else {
    TreeNode* next_node = node_to_erase->right_;
    while (next_node->left_ != nullptr) {
      next_node = next_node->left_;
    }
    // There was an idea to do 'node_to_erase->value_ = next_node->value_'
    // and then delete next_node as a NodeWithOneChild, but ClassThree
    // really made me suffer. So here I replace next_node with node_to_erase
    // and delete the last one.
    if (next_node->parent_ == node_to_erase) {
      next_node->left_ = node_to_erase->left_;
      next_node->parent_ = node_to_erase->parent_;
      if (node_to_erase->parent_ != nullptr) {
        if (node_to_erase->parent_->left_ == node_to_erase) {
          node_to_erase->parent_->left_ = next_node;
        } else {
          node_to_erase->parent_->right_ = next_node;
        }
      }
      if (node_to_erase->left_ != nullptr) {
        node_to_erase->left_->parent_ = next_node;
      }
      if (node_to_erase == root_) {
        root_ = next_node;
      }
    } else {
      next_node->parent_->left_ = next_node->right_;
      if (next_node->right_ != nullptr) {
        next_node->right_->parent_ = next_node->parent_;
      }
      next_node->parent_ = node_to_erase->parent_;
      if (node_to_erase->parent_ != nullptr) {
        if (node_to_erase->parent_->left_ == node_to_erase) {
          node_to_erase->parent_->left_ = next_node;
        } else {
          node_to_erase->parent_->right_ = next_node;
        }
      }
      next_node->right_ = node_to_erase->right_;
      next_node->left_ = node_to_erase->left_;
      if (node_to_erase->right_ != nullptr) {
        node_to_erase->right_->parent_ = next_node;
      }
      if (node_to_erase->left_ != nullptr) {
        node_to_erase->left_->parent_ = next_node;
      }
      if (node_to_erase == root_) {
        root_ = next_node;
      }
    }
    delete node_to_erase;
    size_--;
  }
}

template<typename T>
void BinarySearchTree<T>::erase(const T& value) {
  ConstIterator iterator = find(value);
  if (iterator != end()) {
    this->erase(iterator);
  }
}

// GETTERS
template<typename T>
int BinarySearchTree<T>::count(const T& value) const {
  int count = 0;
  TreeNode* tmp = root_;
  while (tmp != nullptr) {
    if (tmp->value_ < value) {
      tmp = tmp->right_;
    } else if (tmp->value_ == value) {
      count++;
      tmp = tmp->right_;
    } else {
      tmp = tmp->left_;
    }
  }
  return count;
}

template<typename T>
std::vector<T> BinarySearchTree<T>::to_vector() const {
  std::vector<T> result{};
  result.reserve(size_);
  for (auto i = begin(); i != end(); i++) {
    result.push_back(*i);
  }
  return result;
}

// TREE CHANGING
template<typename T>
template<typename U>
void BinarySearchTree<T>::insert(U&& value) {
  auto* node = new TreeNode(std::forward<U>(value));
  this->InsertNode(node);
}

template<typename T>
template<typename... Args>
void BinarySearchTree<T>::emplace(Args&& ... args) {
  auto* node = new TreeNode(std::forward<Args>(args)...);
  this->InsertNode(node);
}

template<typename T>
void BinarySearchTree<T>::InsertNode(BinarySearchTree::TreeNode* node) {
  if (root_ == nullptr) {
    root_ = node;
    size_++;
    return;
  }
  TreeNode* future_parent = root_;
  while (true) {
    if (node->value_ < future_parent->value_) {
      if (future_parent->left_ == nullptr) {
        future_parent->left_ = node;
        node->parent_ = future_parent;
        break;
      }
      future_parent = future_parent->left_;
    } else {
      if (future_parent->right_ == nullptr) {
        future_parent->right_ = node;
        node->parent_ = future_parent;
        break;
      }
      future_parent = future_parent->right_;
    }
  }
  size_++;
}

// COMPARISON OPERATORS
template<typename T>
bool BinarySearchTree<T>::operator==(const BinarySearchTree<T>& rhs) const {
  if (rhs.size_ != size_) {
    return false;
  }
  for (auto i = begin(), j = rhs.begin(); i != end(); ++i, ++j) {
    if (!(*i == *j)) {
      return false;
    }
  }
  return true;
}

template<typename T>
bool BinarySearchTree<T>::operator!=(const BinarySearchTree<T>& rhs) const {
  return !(*this == rhs);
}

// ITERATORS
template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::begin() const {
  TreeNode* tmp = root_;
  while (tmp != nullptr && tmp->left_ != nullptr) {
    tmp = tmp->left_;
  }
  return ConstIterator(const_cast<BinarySearchTree<T>*>(this), tmp);
}

template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::end() const {
  return ConstIterator(const_cast<BinarySearchTree<T>*>(this), nullptr);
}

template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::find(
    const T& value) const {
  TreeNode* tmp = root_;
  while (tmp != nullptr) {
    if (tmp->value_ < value) {
      tmp = tmp->right_;
    } else if (tmp->value_ == value) {
      break;
    } else {
      tmp = tmp->left_;
    }
  }
  return ConstIterator(const_cast<BinarySearchTree<T>*>(this), tmp);
}

// HELPER FUNCTIONS FOR ITERATORS
template<typename T>
typename BinarySearchTree<T>::TreeNode*
BinarySearchTree<T>::ConstIterator::Next() {
  TreeNode* tmp = current_node_;
  if (tmp->right_ == nullptr) {
    TreeNode* tmp_parent = tmp->parent_;
    while (tmp_parent != nullptr && tmp_parent->right_ == tmp) {
      tmp = tmp_parent;
      tmp_parent = tmp->parent_;
    }
    return tmp_parent;
  }
  tmp = tmp->right_;
  while (tmp->left_ != nullptr) {
    tmp = tmp->left_;
  }
  return tmp;
}

template<typename T>
typename BinarySearchTree<T>::TreeNode*
BinarySearchTree<T>::ConstIterator::Previous() {
  TreeNode* tmp = current_node_;
  if (tmp->left_ == nullptr) {
    TreeNode* tmp_parent = tmp->parent_;
    while (tmp_parent != nullptr && tmp_parent->left_ == tmp) {
      tmp = tmp_parent;
      tmp_parent = tmp->parent_;
    }
    return tmp_parent;
  }
  tmp = tmp->left_;
  while (tmp->right_ != nullptr) {
    tmp = tmp->right_;
  }
  return tmp;
}

// COMPARISON OPERATORS
template<typename T>
bool BinarySearchTree<T>::ConstIterator::operator==(
    const BinarySearchTree::ConstIterator& rhs) const {
  return tree_ == rhs.tree_ && current_node_ == rhs.current_node_;
}

template<typename T>
bool BinarySearchTree<T>::ConstIterator::operator!=(
    const BinarySearchTree::ConstIterator& rhs) const {
  return !(*this == rhs);
}

// UNARY OPERATORS
template<typename T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator++() {
  current_node_ = this->Next();
  return *this;
}

template<typename T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator++(int) {
  ConstIterator copy = *this;
  ++(*this);
  return copy;
}

template<typename T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator--() {
  if (current_node_ == nullptr) {
    TreeNode* tmp = tree_->root_;
    while (tmp != nullptr && tmp->right_ != nullptr) {
      tmp = tmp->right_;
    }
    current_node_ = tmp;
  } else {
    current_node_ = this->Previous();
  }
  return *this;
}

template<typename T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator--(int) {
  ConstIterator copy = *this;
  --(*this);
  return copy;
}


#endif  // BINARY_SEARCH_TREE_H_
