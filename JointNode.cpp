#include "JointNode.hpp"
#include <assert.h>
#include <vector>

size_t JointNode::pos = 0;
std::vector<JointNode *> JointNode::all_joints;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
	all_joints.push_back( this );
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	
	m_stack_x.clear();
	m_stack_x.push_back( init );
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	
	m_stack_y.clear();
	m_stack_y.push_back( init );
}

void JointNode::rotate_x(double deg) {
    m_joint_x.init += deg;
    if (m_joint_x.init > m_joint_x.max) {
        m_joint_x.init = m_joint_x.max;
    }
    if (m_joint_x.init < m_joint_x.min) {
        m_joint_x.init = m_joint_x.min;
    }
    
    if (m_joint_x.init < -180.0 ) {
        m_joint_x.init += 360.0;
    } 
    if (m_joint_x.init > 180.0 ) {
        m_joint_x.init -= 360.0;
    } 
}

void JointNode::rotate_y(double deg) {
    m_joint_y.init += deg;
    if (m_joint_y.init > m_joint_y.max) {
        m_joint_y.init = m_joint_y.max;
    }
    if (m_joint_y.init < m_joint_y.min) {
        m_joint_y.init = m_joint_y.min;
    }
    if (m_joint_y.init < -180.0 ) {
        m_joint_y.init += 360.0;
    } 
    if (m_joint_y.init > 180.0 ) {
        m_joint_y.init -= 360.0;
    } 
}

void JointNode::save_state() {
    if ( m_stack_x.size() > pos + 1 ) {
        // 2 stacks should have the same size
        m_stack_x.resize( pos + 1 );
        m_stack_y.resize( pos + 1 );
    }

    m_stack_x.push_back( m_joint_x.init );
    m_stack_y.push_back( m_joint_y.init );       
}

bool JointNode::redo() {
    assert( m_stack_x.size() == m_stack_y.size() );
    assert( m_stack_x.size() > pos );
    if ( m_stack_x.size() == pos + 1 ) {
        // already on top
        return false;
    } 
    
    m_joint_x.init = m_stack_x[pos + 1];
    m_joint_y.init = m_stack_y[pos + 1];
    
    return true;   
}

bool JointNode::undo() {
    assert( m_stack_x.size() == m_stack_y.size() );
    assert( m_stack_x.size() > pos );
    if ( 0 == pos ) {
        return false;
    }

    m_joint_x.init = m_stack_x[pos - 1];
    m_joint_y.init = m_stack_y[pos - 1];
    
    return true;
}

void JointNode::reset() {
    m_stack_x.resize( 1 );
    m_stack_y.resize( 1 );
    m_joint_x.init = m_stack_x[0];
    m_joint_y.init = m_stack_y[0];
}

void JointNode::save_all_states() {
    for ( JointNode *node: all_joints ) {
        node->save_state();
    }
    pos++;
}

bool JointNode::redo_all() {
    for ( JointNode *node: all_joints ) {
        if ( !node->redo() ) return false;
    }
    pos++;
    return true;
}

bool JointNode::undo_all() {
    for ( JointNode *node: all_joints ) {
        if ( !node->undo() ) return false;
    }
    pos--;
    return true;
}

void JointNode::reset_all() {
    for ( JointNode *node: all_joints ) {
        node->reset();
    }
    pos = 0;
}

















