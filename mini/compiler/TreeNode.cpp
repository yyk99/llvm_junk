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

std::string TreeNode::oper_to_string() const
{
    return token_to_string(oper);
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
