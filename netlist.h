#ifndef _NETLIST_H
#define _NETLIST_H


#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <assert.h>
#include <map>
#include <sstream>
#include "parse.h"

class netlist;
class gate;
class net;
class pin;

//net类
class net
{
	friend class netlist;
	friend class pin;
	std::string net_name_;
	net(std::string n) :net_name_(n){};
	net(const net &);
	net &operator=(const net &);
	std::list<pin *> connections_;

public:
	void append_pin(pin*);
	bool retrieve_logic_value();//计算逻辑数值
	int value_;
};

//pin类
class pin
{
public:
	friend class gate;
	friend class netlist;
	pin(){}
	pin(const pin &);
	pin &operator=(const pin &);
	gate *gate_;
	size_t pin_index_;
	std::vector<net*> nets_;
	net* get_net();
	int path;//输入输出逻辑值

	bool state;//state
	bool get_value();//获得数值
	bool create(gate *g, size_t pin_index, const evl_pin &, const std::map<std::string, net *> &);
	void set_as_input()//设置为输入
	{ 
		path = 1; 
	}
	void set_as_output()//设置为输出
	{ 
		path = 0;
	}
};

//gate类
class gate
{
private:
	virtual bool validate_structural_semantics() = 0;

public:
	friend class netlist;
	gate(){}
	gate(const gate &);
	gate &operator=(const gate &);
	~gate();
	std::string type_, name_;
	std::vector<pin*> pins_;
    gate(std::string type, std::string name);

	bool create_pins(const evl_pins &pins, const std::map<std::string, net *> &nets_table_, const evl_wires_table &wires_table);
	bool create(const evl_comp &, const std::map<std::string, net *> nets_table_, const evl_wires_table &);
	bool create_pin(const evl_pin &, size_t pin_index, const std::map<std::string, net *> &nets_table_, const evl_wires_table &);
	std::string get_type() const;
	std::string get_name() const;
	void display(std::ostream &out)const;
	virtual bool compute()
	{
		return true;
	}
	virtual bool get_next_state()
	{ 
		return true; 
	}
};

//netlist类
class netlist
{
public:
	netlist(){};
	netlist(const netlist &);
	netlist &operator=(const netlist &);
	std::list<gate*> gates_;
	std::list<net*> nets_;
	std::map<std::string, net*> nets_table_;

	bool create(const evl_wires &, const evl_comps &, const evl_wires_table &);
	bool create_nets(const evl_wires &);
	bool create_gates(const evl_comps &, const evl_wires_table &);
	~netlist();
	void display(std::string & file_name, std::string module_name, std::ostream &out);
	bool create_gate(const evl_comp &, const evl_wires_table &);
	void create_net(std::string);
	void save(std::string &);
	bool simulation(int times, std::string &evl_file);//仿真输出
	bool simulate_evl_out(std::list<gate*>::iterator it_out, std::ofstream &fout, int t);//evl_output门仿真
	bool simulate_evl_in(std::list<gate*>::iterator it_in, std::ofstream &out, int t);//evl_input门仿真
	std::string convert_output(std::vector<int>& dout);
};

//生成net的名字
std::string make_net_name(std::string, int);
#endif

