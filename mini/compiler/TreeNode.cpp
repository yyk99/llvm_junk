//
//
//

#include "TreeNode.h"
#include "parser.h"

TreeIdentNode::TreeIdentNode(const char *name)
  : TreeNode(0, 0, IDENT), id(name)
{
}

TreeNumericalNode::TreeNumericalNode(int n)
  : TreeNode(0, 0, NUMBER), num(n)
{}

