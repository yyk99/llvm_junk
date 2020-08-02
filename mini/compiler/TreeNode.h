#ifndef __TREENODE_H
#define __TREENODE_H

#include <string>

class TreeNode {
public:
    std::string id;
    TreeNode()
    {
    }
    TreeNode(const char *txt) : id(txt)
    {}
};

#endif
