#pragma once

#include "SceneNode.hpp"
#include <vector>

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);
	
	void rotate_x(double deg);
	void rotate_y(double deg);
	
	static void save_all_states();
	static bool redo_all();
	static bool undo_all();
	static void reset_all();

	struct JointRange {
		double min, init, max;
	};

	JointRange m_joint_x, m_joint_y;
	
private:
    static std::vector<JointNode *> all_joints;
    static size_t pos;	
    
	std::vector<double> m_stack_x, m_stack_y;
	
	// Commands on a single JointNode. They don't update global vars (pos) 
	void save_state();
	bool redo();
	bool undo();
	void reset();
};
