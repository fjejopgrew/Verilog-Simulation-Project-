#include "netlist.h"
#include "simulation.h"

//在connection中储存pin
void net::append_pin(pin *p){
	connections_.push_back(p);
}

//取得net的逻辑数值
bool net::retrieve_logic_value()
{
	if (value_ == -1)
	{
		//std::list<pin*>::iterator it;
		for (std::list<pin*>::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			if ((*it)->path == 0)	
				return (*it)->get_value();
				//break;
		}
	}
	else if (value_ == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//创建pin
bool pin::create(gate *g, size_t pin_index, const evl_pin &ep, const std::map<std::string, net *> &nets_table_)
{
	gate_ = g;
	pin_index_ = pin_index;
	if (ep.bus_msb == -1)
	{
		net* net = nets_table_.find(ep.name)->second;
		nets_.push_back(net);
		net->append_pin(this);
	}
	else
	{
		for (size_t i = ep.bus_lsb; i <= ep.bus_msb; ++i)
		{
			net* net = nets_table_.find(make_net_name(ep.name, i))->second;
			nets_.push_back(net);
			net->append_pin(this);
		}
	}
	return true;
}

//获得pin的net
net* pin::get_net() {
	return nets_[0];
}

//获得pin的value
bool pin::get_value()
{
	if (path == 1)
	{
		return nets_[0]->retrieve_logic_value();
	}
	else if (path == 0)
	{

		return gate_->compute();
	}
}

//gate的语法结构
bool gate::validate_structural_semantics() {
	assert(false); 
	return false;
}

//创建gate
bool gate::create(const evl_comp &c, const std::map<std::string, net *> nets_table_, const evl_wires_table &wires_table)
{
	name_ = c.name;
	type_ = c.type;
	size_t index = 0;
	for (evl_pins::const_iterator it = c.pins.begin(); it != c.pins.end(); ++it)
	{
		create_pin(*it, index, nets_table_, wires_table);
		++index;
	}
	return true;
	return validate_structural_semantics();

}

//在gate中创建pin
bool gate::create_pin(const evl_pin &ep, size_t pin_index, const std::map<std::string, net *> &nets_table_, const evl_wires_table &wires_table)
{
	pin *p = new pin;
	pins_.push_back(p);
	return p->create(this, pin_index, ep, nets_table_);
}

//在gate中创建pins
bool gate::create_pins(const evl_pins &pins, const std::map<std::string, net *> &nets_table_, const evl_wires_table &wires_table) {

	size_t pin_index = 0;
	for (evl_pins::const_iterator i = pins.begin(); i != pins.end(); ++i) {
		create_pin(*i, pin_index, nets_table_, wires_table);
		++pin_index;
	}

	return validate_structural_semantics();
}

//创建netlist
bool netlist::create(const evl_wires &wires, const evl_comps &comps, const evl_wires_table &wires_table)
{
	return create_nets(wires) && create_gates(comps, wires_table);
}

//在netlist中创建nets
bool netlist::create_nets(const evl_wires &wires)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		if (it->width == 1)
		{
			create_net(it->name);
		}
		else
		{
			for (int i = 0; i < (it->width); ++i)
			{
				create_net(make_net_name(it->name, i));
			}
		}
	}
	return true;
}

//在netlist中创建gates
bool netlist::create_gates(const evl_comps &comps, const evl_wires_table &wires_table)
{
	for (evl_comps::const_iterator it = comps.begin(); it != comps.end(); ++it)
	{
		create_gate(*it, wires_table);
	}
	return true;
}

//显示结果
void netlist::display(std::string & file_name, std::string module_name, std::ostream &out)
{
	out << "module " << module_name << std::endl;
	out << "nets " << nets_.size() << std::endl;

	for (std::list<net*>::iterator it = nets_.begin(); it != nets_.end(); ++it)
	{
		out << "  net " << (*it)->net_name_ << " " << (*it)->connections_.size() << std::endl;
		for (std::list<pin*>::iterator itp = ((*it)->connections_).begin(); itp != ((*it)->connections_).end(); ++itp)
		{
			if ((*itp)->gate_->name_ == "")
			{
				out << "    " << (*itp)->gate_->type_ << " " << (*itp)->pin_index_ << std::endl;
			}
			else
			{
				out << "    " << (*itp)->gate_->type_ << " " << (*itp)->gate_->name_ << " " << (*itp)->pin_index_ << std::endl;
			}
		}
	}

	out << "components " << gates_.size() << std::endl;
	for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
	{
		if ((*it)->name_ == "")
		{
			out << "  component " << (*it)->type_ << " " << ((*it)->pins_).size() << std::endl;
		}
		else
		{
			out << "  component " << (*it)->type_ << " " << (*it)->name_ << " " << ((*it)->pins_).size() << std::endl;
		}
		for (std::vector<pin *>::iterator it_p = (*it)->pins_.begin(); it_p != (*it)->pins_.end(); ++it_p)
		{
			out << "    pin" << " " << (*it_p)->nets_.size();

			for (std::vector<net*>::iterator it_n = (*it_p)->nets_.begin(); it_n != (*it_p)->nets_.end(); ++it_n)
			{
				out << " " << (*it_n)->net_name_;
			}
			out << std::endl;
		}
	}
}

//在netlist中创建gate
bool netlist::create_gate(const evl_comp &c, const evl_wires_table & wires_table)
{
	if (c.type == "evl_output")
	{
		gate *g = new evl_output_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_input")
	{
		gate *g = new evl_input_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_dff")
	{
		gate *g = new evl_flip_flop(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_clock")
	{
		gate *g = new evl_clock_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "and")
	{
		gate *g = new and_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "or")
	{
		gate *g = new or_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "xor")
	{
		gate *g = new xor_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "not")
	{
		gate *g = new not_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "buf")
	{
		gate *g = new buf_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_one")
	{
		gate *g = new evl_one_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_zero")
	{
		gate *g = new evl_zero_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_lut")
	{
		gate *g = new evl_lut_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "tris")
	{
		gate *g = new tris_gate(c.name);
		gates_.push_back(g);
	}
	else
	{
		std::cerr << "Unknown gate type, can not create gate" << std::endl;
		return false;
	}
	return gates_.back()->create_pins(c.pins, nets_table_, wires_table);
}

//创建net元素
void netlist::create_net(std::string net_name)
{
	assert(nets_table_.find(net_name) == nets_table_.end());
	net *p = new net(net_name);
	nets_table_[net_name] = p;
	nets_.push_back(p);
}

//转换字符为16进制
std::string netlist::convert_output(std::vector<int>& num)
{
	while (num.size() % 4 != 0)
		num.insert(num.begin(), 0);
	std::string digit;
	for (int i = 0, sum = 0; i < num.size(); i += 4)
	{
		sum = num[i] * 8 + num[i + 1] * 4 + num[i + 2] * 2 + num[i + 3] * 1;
		switch (sum)
		{
		case 0:digit += "0"; break;
		case 1:digit += "1"; break;
		case 2:digit += "2"; break;
		case 3:digit += "3"; break;
		case 4:digit += "4"; break;
		case 5:digit += "5"; break;
		case 6:digit += "6"; break;
		case 7:digit += "7"; break;
		case 8:digit += "8"; break;
		case 9:digit += "9"; break;
		case 10:digit += "A"; break;
		case 11:digit += "B"; break;
		case 12:digit += "C"; break;
		case 13:digit += "D"; break;
		case 14:digit += "E"; break;
		case 15:digit += "F"; break;
		default: digit += "?"; break;
		}
	}
	return digit;
}

//仿真输出
bool netlist::simulation(int times, std::string &evl_file)
{
	std::list<gate*>::iterator it_out;
	for (it_out = gates_.begin(); it_out != gates_.end(); ++it_out)
	{
		if ((*it_out)->type_ == "evl_output")
		{
			break;
		}
	}
	if (it_out == gates_.end())
	{
		std::cerr << "no output gate in module" << std::endl;
		return false;
	}
	std::string out_file = evl_file + "." + (*it_out)->name_ + ".evl_output";
	std::ofstream out_out(out_file.c_str());
	out_out << (*it_out)->pins_.size() << std::endl;
	for (int i = 0; i < (*it_out)->pins_.size(); ++i)
	{
		out_out << (*it_out)->pins_[i]->nets_.size() << std::endl;
	}
	simulate_evl_out(it_out, out_out, times);

	std::list<gate*>::iterator it_in;
	for (it_in = gates_.begin(); it_in != gates_.end(); ++it_in)
	{
		if ((*it_in)->type_ == "evl_input")
		{
			break;
		}
	}
	if (it_in == gates_.end())
	{
		std::cerr << "no input gate in module" << std::endl;
		return false;
	}
	std::string out_file2 = evl_file + "." + (*it_in)->name_ + ".evl_input1";
	std::ofstream out2(out_file2.c_str());
	out2 << (*it_in)->pins_.size()<<" ";
	for (int i = 0; i < (*it_in)->pins_.size(); ++i)
	{
		out2 << (*it_in)->pins_[i]->nets_.size() << " ";
	}
	out2 << std::endl;
	simulate_evl_in(it_in, out2, times);
	return true;
}

//evl_output仿真
bool netlist::simulate_evl_out(std::list<gate*>::iterator it_out, std::ofstream &out, int t)
{
	for (int times = 1; times <= t; times++)
	{
		for (std::map<std::string, net*>::iterator it = nets_table_.begin(); it != nets_table_.end(); ++it)
		{
			it->second->value_ = -1;
		}
		for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
		{
			//对于每个evl_dff gate, 计算它的nex_state_
			if ((*it)->type_ == "evl_dff")
			{
				(*it)->pins_[0]->get_net()->value_ = (*it)->get_next_state();
			}
		}

		//完成数据类型转换并且输出
		for (int i = 0; i < (*it_out)->pins_.size(); ++i)
		{
			std::vector<int> num;
			for (int j = 0; j < (*it_out)->pins_[i]->nets_.size(); ++j)
			{
				num.insert(num.begin(), (*it_out)->pins_[i]->nets_[j]->retrieve_logic_value());
			}
			out << convert_output(num) << " ";
		}
		out << std::endl;
		//计算每个fliflop运行以后的数值
		for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
		{
			if ((*it)->type_ == "evl_dff")
			{
				(*it)->compute();
			}
		}
	}
	out.close();
	return true;
}

//evl_input仿真
bool netlist::simulate_evl_in(std::list<gate*>::iterator it_in, std::ofstream &out, int t)
{
	for (int times = 1; times <= t; times++)
	{
		out << times << " ";
		for (std::map<std::string, net*>::iterator it = nets_table_.begin(); it != nets_table_.end(); ++it)
		{
			it->second->value_ = -1;
		}
		for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
		{
			if ((*it)->type_ == "evl_dff")
			{
				(*it)->pins_[0]->nets_[0]->value_ = (*it)->get_next_state();
			}
		}
		//完成数据类型转换并且输出
		for (int i = 0; i < (*it_in)->pins_.size(); ++i)
		{
			std::vector<int> num;
			for (int j = 0; j < (*it_in)->pins_[i]->nets_.size(); ++j)
			{
				num.insert(num.begin(), (*it_in)->pins_[i]->nets_[j]->retrieve_logic_value());
			}
			out << convert_output(num) << " ";
		}
		out << std::endl;
		//计算每个fliflop运行以后的数值
		for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
		{
			if ((*it)->type_ == "evl_dff")
			{
				(*it)->compute();
			}
		}
	}
	out.close();
	return true;
}


//删除gate
gate::~gate()
{
	for (size_t i = 0; i < pins_.size(); ++i)
		delete pins_[i];
}

//gate的构造
gate::gate(std::string t, std::string n)
	: type_(t), name_(n) {
}

//删除netlist
netlist::~netlist()
{
	for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)
		delete *it;
	for (std::list<net*>::iterator it = nets_.begin(); it != nets_.end(); ++it)
		delete *it;
}

//生成net的名字
std::string make_net_name(std::string wire_name, int i)
{
	assert(i >= 0);
	std::ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}



