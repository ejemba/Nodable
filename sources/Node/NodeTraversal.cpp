#include "NodeTraversal.h"
#include "Wire.h"

#include <iostream>

using namespace Nodable;


#define DISABLE_NODE_TRAVERSAL_MESSAGE_LOG

#ifdef DISABLE_NODE_TRAVERSAL_MESSAGE_LOG
#undef LOG_MESSAGE
#define LOG_MESSAGE
#endif

Result NodeTraversal::Update(Node* _rootNode) {
    LOG_MESSAGE("NodeTraversal::Update %s \n", _rootNode->getLabel() );
    std::vector<Node*> traversed;
    return NodeTraversal::UpdateRecursively(_rootNode, traversed);
}

Result NodeTraversal::SetDirty(Node* _rootNode) {
    LOG_MESSAGE("NodeTraversal::SetDirty %s \n", _rootNode->getLabel() );
    std::vector<Node*> traversed;
    return NodeTraversal::SetDirtyRecursively(_rootNode, traversed);
}

Result NodeTraversal::SetDirtyRecursively(Node* _node, std::vector<Node*>& _traversed) {

    Result result;
    
    LOG_MESSAGE("NodeTraversal::SetDirtyEx");
    
    // Check if we already updated this node
    auto alreadyUpdated = std::find( _traversed.cbegin(), _traversed.cend(), _node ) != _traversed.cend();
    if( !alreadyUpdated )
    {
        _traversed.push_back(_node);

        _node->setDirty();

        for (auto wire : _node->getWires() )
        {

            if (wire->getSource()->getOwner() == _node &&
                wire->getTarget() != nullptr)
            {
                auto targetNode = reinterpret_cast<Node*>(wire->getTarget()->getOwner());
                
                auto r = NodeTraversal::SetDirtyRecursively(targetNode, _traversed);
                if( r == Result::Failure )
                    return Result::Failure;
            }
        };

        result = Result::Success;
    } else {
        result = Result::Failure;
    }

    return result;
}

Result NodeTraversal::UpdateRecursively(Node* _node, std::vector<Node*>& _traversed) {
    
    Result result;
    LOG_MESSAGE("NodeTraversal::UpdateEx ");
    
    // Check if we already updated this node
    auto alreadyUpdated = std::find( _traversed.cbegin(), _traversed.cend(), _node ) != _traversed.cend();
    if( !alreadyUpdated )
    {
        _traversed.push_back(_node);

        // Evaluates only if dirty flag is on
        if (_node->isDirty())
        {
            // first we need to evaluate each input and transmit its results thru the wire
            auto wires = _node->getWires();
            for (auto wire : wires)
            {
                auto wireTarget = wire->getTarget();
                auto wireSource = wire->getSource();

                if ( _node->has(wireTarget) &&
                    wireSource != nullptr) 
                {
                    /* update the source entity */
                    auto sourceNode = reinterpret_cast<Node*>(wireSource->getOwner());
                    NodeTraversal::UpdateRecursively(sourceNode, _traversed);
                    
                    /* transfert the freshly updated value from source to target member */
                    wireTarget->updateValueFromInputMemberValue();
                }
            }

            // Update
            _node->update();
            _node->setDirty(false);
       
            /* Set dirty all childrens
            for (auto wire : wires)
            {

                if (wire->getSource()->getOwner() == _node &&
                    wire->getTarget() != nullptr)
                {
                    auto targetNode = reinterpret_cast<Node*>(wire->getTarget()->getOwner());
                    NodeTraversal::SetDirty(targetNode);
                }
            };*/


        }
        result = Result::Success;

    } else {
        result = Result::Failure;
        LOG_WARNING("Unable to update Node %s, cycle detected.", _node->getLabel() );
    }

    return result;
}