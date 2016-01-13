#ifndef simulation_simulation_h
#define simulation_simulation_h
#include "netlist.h"

//and门类
class and_gate : public gate
{
public:
	and_gate(std::string name):gate("and",name){}
	bool compute();
	bool validate_structural_semantics();
};

//or门类
class or_gate : public gate
{
public:
	or_gate(std::string name):gate("or",name){}
	bool compute();
	bool validate_structural_semantics();
};

//xor门类
class xor_gate : public gate
{
public:
	xor_gate(std::string name):gate("xor",name){}
	bool compute();
	bool validate_structural_semantics();
};

//buf门类
class buf_gate : public gate
{
public:
	buf_gate(std::string name):gate("buf",name){}
	bool compute();
	bool validate_structural_semantics();
};

//flipflop门类
class evl_flip_flop : public gate
{
	bool state_,next_state_;
public:
	evl_flip_flop(std::string name) :gate("evl_dff", name),state_(false),next_state_(false){}
	bool compute();
	bool validate_structural_semantics();
	bool get_next_state()
	{
		return next_state_;
	}
};

//not门类
class not_gate : public gate
{
public:
	not_gate(std::string name):gate("not",name){}
	bool compute();
	bool validate_structural_semantics();
};

//evl_one门类
class evl_one_gate : public gate
{
public:
	evl_one_gate(std::string name) :gate("evl_one", name){}
	bool compute();
	bool validate_structural_semantics();
};

//evl_zero门类
class evl_zero_gate : public gate
{
public:
	evl_zero_gate(std::string name) :gate("evl_zero", name){}
	bool compute();
	bool validate_structural_semantics();
};

//evl_input门类
class evl_input_gate : public gate
{
public:
	evl_input_gate(std::string name) :gate("evl_input", name){}
	bool validate_structural_semantics();
};

//evl_output门类
class evl_output_gate : public gate
{
public:
	evl_output_gate(std::string name) :gate("evl_output", name){}
	bool validate_structural_semantics();
};

//evl_clock_gate门类
class evl_clock_gate : public gate
{
public:
	evl_clock_gate(std::string name) :gate("evl_clock", name){}
	bool compute();
	bool validate_structural_semantics();
};

//evl_lut_gate门类
class evl_lut_gate : public gate
{
public:
	evl_lut_gate(std::string name) :gate("evl_lut", name){}
	bool compute();
	bool validate_structural_semantics();
};

//tris_gate门类
class tris_gate : public gate
{
public:
	tris_gate(std::string name) :gate("tris", name){}
	bool compute();
	bool validate_structural_semantics();
};


#endif
























