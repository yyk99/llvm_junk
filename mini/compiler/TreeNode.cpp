//
//
//

#include "TreeNode.h"
#include "parser.h"

TreeIdentNode::TreeIdentNode(const char *name)
  : TreeNode(0, 0, IDENT), id(name)
{
}

