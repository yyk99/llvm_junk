#ifndef __TREENODE_H
#define __TREENODE_H

#include <string>

class TreeNode {
public:
    TreeNode * left;
    TreeNode * right;
    int oper;

    TreeNode() : left{0}, right{0}, oper{0}
    {
    }

    TreeNode(TreeNode *l, TreeNode *r, int o) : left{l}, right{r}, oper{o}
    {}

    virtual std::string show() const = 0;

    virtual std::string oper_to_string() const;
};


class TreeIdentNode : public TreeNode {
public:
    std::string id;

    TreeIdentNode(const char *name);
    virtual std::string show() const { return id; }
};

class TreeNumericalNode : public TreeNode {
public:
    int num; 

    TreeNumericalNode(int n);
    virtual std::string show() const { return std::to_string(num); }
};

class TreeDNumericalNode : public TreeNode {
public:
    double num; 

    TreeDNumericalNode(double n) :TreeNode(), num(n) {}
    virtual std::string show() const { return std::to_string(num); }
};

class TreeTextNode : public TreeNode {
public:
    std::string text;

    TreeTextNode(const char *t, size_t len) :TreeNode(), text(t, len) {}
    virtual std::string show() const { return text; }
};

class TreeBooleanNode : public TreeNode {
public:
    bool num;

    TreeBooleanNode(bool b) : TreeNode{}, num(b) {}
    virtual std::string show() const { return std::to_string(num); }
};

class TreeBinaryNode : public TreeNode {
public:
    TreeBinaryNode(TreeNode *left, TreeNode *right, int op) : TreeNode(left, right, op) {}

    virtual std::string show() const {
        return
            oper_to_string()
            + "("
            + (left ? left->show(): "<null>")
            + " "
            + (right ? right->show() : "<null>")
            + ")"
            ;
    }
};

class TreeUnaryNode : public TreeNode {
public:
    TreeUnaryNode(TreeNode *left, int op) : TreeNode(left, 0, op) {}

    virtual std::string show() const {
        return
            oper_to_string()
            + "("
            + (left ? left->show(): "<null>")
            + ")"
            ;
    }
};

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:

#endif
