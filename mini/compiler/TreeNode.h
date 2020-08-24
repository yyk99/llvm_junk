#ifndef __TREENODE_H
#define __TREENODE_H

#include <string>

class TreeNode {
public:
    TreeNode * left;
    TreeNode * right;

    TreeNode() : left{0}, right{0}
    {
    }

    TreeNode(TreeNode *l, TreeNode *r) : left{l}, right{r}
    {}

    virtual std::string show() const = 0;
};


class TreeIdentNode : public TreeNode {
public:
    std::string id;

    TreeIdentNode(const char *name) :TreeNode(), id(name) {}
    virtual std::string show() const { return id; }
};

class TreeNumericalNode : public TreeNode {
public:
    int num; 

    TreeNumericalNode(int n) :TreeNode(), num(n) {}
    virtual std::string show() const { return std::to_string(num); }
};

class TreeBooleanNode : public TreeNode {
public:
    bool num;

    TreeBooleanNode(bool b) : TreeNode{}, num(b) {}
    virtual std::string show() const { return std::to_string(num); }
};

class TreeBinaryNode : public TreeNode {
public:
    int oper;
    TreeBinaryNode(TreeNode *left, TreeNode *right, int op) : TreeNode(left, right), oper(op) {}

    virtual std::string show() const { return left->show() + " " + std::to_string(oper) + " " + right->show(); }
};

class TreeUnaryNode : public TreeNode {
public:
    int oper;
    TreeUnaryNode(TreeNode *left, int op) : TreeNode(left, 0), oper(op) {}

    virtual std::string show() const { return left->show() + " " + std::to_string(oper); }
};

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:

#endif
