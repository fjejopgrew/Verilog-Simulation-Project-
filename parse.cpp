#include "parse.h"

//从一行中取得该行的tokens
bool extract_tokens_from_line(std::string line, int line_no, evl_tokens &tokens)
{ 
	for (size_t i = 0; i < line.size();)
	{
		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				std::cerr << "LINE " << line_no
					<< ": a single / is not allowed" << std::endl;
				return false;
			}
			if (line[i] == '/')
				break; // // skip the rest of the line by exiting the loop 跳过注释行
		}  
		else if (isspace(line[i]))
		{
			{
				++i; // skip this space character 跳过空格
				continue; // skip the rest of the iteration 继续本轮循环
			}
		}
		else if ((line[i] == '(') || (line[i] == ')') || (line[i] == '[') || (line[i] == ']')
			|| (line[i] == ':') || (line[i] == ';')
			|| (line[i] == ','))
		{
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::SINGLE;
			token.str = std::string(1, line[i]);
			tokens.push_back(token); // // 向容器中添加此SINGLE token
			++i;
			continue;// 继续添加
		}
		else if (isalpha(line[i]) || 
			((line[i] >= 'a') && (line[i] <= 'z'))       // a to z
			|| ((line[i] >= 'A') && (line[i] <= 'Z'))    // A to Z
			|| (line[i] == '_') || (line[i] == '\\')
			|| (line[i] == '.'))
		{
			size_t name_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!(((line[i] >= 'a') && (line[i] <= 'z'))
					|| ((line[i] >= 'A') && (line[i] <= 'Z'))
					|| ((line[i] >= '0') && (line[i] <= '9'))
					|| (line[i] == '_') || (line[i] == '\\') || (line[i] == '.')))//其他符号
				{
					break;  // [name_begin, i) is the range for the token 此为token的长度
				}
			}
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NAME;
			token.str = line.substr(name_begin, i - name_begin);//将此NAME token储存为string
			tokens.push_back(token);//将此NAME类型token返回容器
			continue;
		}
		else if (((line[i] >= '0') && (line[i] <= '9')))
		{
			size_t number_begin = i;
			for (++i; i<line.size(); ++i)
			{
				if (!((line[i] >= '0') && (line[i] <= '9')))
				{
					break; //// [number_begin, i) is the range for the token 此为NUMBER token长度
				}


			}
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NUMBER;
			token.str = line.substr(number_begin, i - number_begin);//将此NUMBER token存入tokens容器中
			tokens.push_back(token);
			continue;

		}
		else {
			std::cerr << "LINE " << line_no
				<< ": invalid character" << std::endl;
			return false;
		}
	}
	return true; // nothing left
}

//从文件中提取tokens
bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens)
{
	std::ifstream input_file(file_name.c_str());
	if (!input_file)
	{
		std::cerr << "I can’t read " << file_name << "." << std::endl;
		return false;
	}
	tokens.clear(); // be defensive, make it make it good
	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no)
	{
		if (!extract_tokens_from_line(line, line_no, tokens))
		{
			return false;
		}
	}
	return true;
}

//将tokens推入statement
bool push_tokens_to_statemnet(evl_tokens &statement_tokens, evl_tokens &tokens)
{
	assert(statement_tokens.empty());//确保容器不为空，否则中断程序
	assert(!tokens.empty());//确保token存在，否则中断程序
	for (; !tokens.empty();)//如果tokens为空
	{
		statement_tokens.push_back(tokens.front());
		tokens.erase(tokens.begin());
		if (statement_tokens.back().str == ";")
			break;
	}
	if (statement_tokens.back().str != ";")
	{
		std::cerr << "can not find a ;" << std::endl;
		return false;
	}
	return true;
}

//显示tokens
void display_tokens(std::ofstream &out, const evl_tokens &tokens)
{
	for (evl_tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		if (it->type == evl_token::SINGLE)
		{
			out << "SINGLE " << it->str << std::endl;
		}
		else if (it->type == evl_token::NAME)
		{
			out << "NAME " << it->str << std::endl;
		}
		else 
		{
			out << "NUMBER " << it->str << std::endl;
		} 
	} 
}

//将tokens储存在file中
bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens)
{
	file_name += ".tokens";
	std::ofstream output_file(file_name.c_str());
	display_tokens(output_file, tokens);
	output_file.close();
	return true;
}

//查看是否符合pin规范
bool pin_check_rules(evl_wires &wires, evl_pin &pin)
{
	//创建两个迭代器
	evl_wires::const_iterator it1;
	evl_wires::const_iterator it = wires.begin();
	for (it1 = wires.begin(); it1 != wires.end(); it1++)
	{
		if (it1->name == pin.name)
		{
			it = it1;
		}
	}
			if (it == wires.end())
			{
				std::cout << "fail to find pin " << pin.name << " in wires " << std::endl;
				return false;
			}
			if (it->width == 1) 
			{
				if (!(pin.bus_lsb == -1) && (pin.bus_msb == -1))
				{
					std::cout << " fail in width 1 wire " << pin.name << std::endl;
					return false;
				}
			}
			else 
			{

				if ((pin.bus_lsb == -1) && (pin.bus_msb == -1))
				{
					pin.bus_msb = it->width - 1;
					pin.bus_lsb = 0;
				}
				else if ((pin.bus_msb ==-1) && (pin.bus_lsb != -1))
				{
					if (!((it->width > pin.bus_msb) && (pin.bus_msb >= pin.bus_lsb) && (pin.bus_lsb >= 0)))
					{
						std::cout << "Fail in bus wire " << pin.name << std::endl;
						return false;
					}
				}
				else if ((pin.bus_msb != -1) && (pin.bus_lsb == -1))
				{
					if ((it->width > pin.bus_msb) && (pin.bus_msb >= 0))
					{
						pin.bus_lsb = pin.bus_msb;
					}
				}
			}
		return true;
}

//识别wire statement
bool process_wire_statement(evl_wires &wires, evl_statement &s)
{
	assert(s.type == evl_statement::WIRE);
	enum state_type { INIT, WIRE, DONE, WIRES, WIRE_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE };
	evl_wire wire;
	state_type state = INIT;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		if (state == INIT)
		{
			if (t.str == "wire")
			{
				state = WIRE;
			}
			else
			{
				std::cerr << "Need 'wire' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == WIRE)
		{
			if (t.type == evl_token::NAME)
			{
				wire.width = 1;
				wire.name = t.str;
				wires.push_back(wire);
				state = WIRE_NAME;
			}
			else if (t.str == "[")
			{
				state = BUS;
			}
			else
			{
				std::cerr << "Need NAME or '[' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER)
			{
				wire.width = atoi(t.str.c_str()) + 1;
				state = BUS_MSB;
			}
			else
			{
				std::cerr << "Need NUMBER but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_MSB)
		{
			if (t.str == ":")
			{
				state = BUS_COLON;
			}
			else
			{
				std::cerr << "Need ':' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_COLON)
		{
			if (t.str == "0")
			{
				state = BUS_LSB;
			}
			else
			{
				std::cerr << "Need '0' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_LSB)
		{
			if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ']' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_DONE)
		{
			if (t.type == evl_token::NAME)
			{
				wire.name = t.str;
				wires.push_back(wire);
				state = WIRE_NAME;
			}
			else
			{
				std::cerr << "Need NAME but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == WIRE_NAME)
		{
			if (t.str == ",")
			{
				state = WIRES;
			}
			else if (t.str == ";")
			{
				state = DONE;
			}
			else
			{
				std::cerr << "Need ',' or ';' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}

		}
		else if (state == WIRES)
		{
			if (t.type == evl_token::NAME)
			{
				wire.name = t.str;
				wires.push_back(wire);
				state = WIRE_NAME;
			}
			else
			{
				std::cerr << "Need 'NAME' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else
		{
			assert(false);
		}
	}
	if (!s.tokens.empty() || (state != DONE))
	{
		std::cerr << "Something wrong with the statement" << std::endl;
		return false;
	}
	return true;
}

//识别component statement
bool process_comp_statement(evl_comps &comps, evl_statement &s, evl_wires&wires)
{

	assert(s.type == evl_statement::COMPONENT);//中断
	enum state_type { INIT, TYPE, DONE, NAME, PINS, PIN_NAME, PINS_DONE, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE };
	evl_comp comp;// evl_pins pins;
	evl_pin pin;
	state_type state = INIT;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		//use branches here to compute state transitions
		if (state == INIT)
		{
			if (t.type == evl_token::NAME)
			{
				comp.type = t.str;
				comp.name = "";
				state = TYPE;
			}
			else
			{
				std::cerr << "Need NAME but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == TYPE)
		{
			if (t.type == evl_token::NAME)
			{
				comp.name = t.str;
				state = NAME;
			}
			else if (t.str == "(")
			{
				state = PINS;
			}
			else
			{
				std::cerr << "Need NAME or '[' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == NAME)
		{
			if (t.str == "(")
			{
				state = PINS;
			}
			else
			{
				std::cerr << "Need NUMBER but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == PINS)
		{
			if (t.type == evl_token::NAME)
			{
				pin.name = t.str;
				pin.bus_msb = -1;
				pin.bus_lsb = -1;
				state = PIN_NAME;
			}
			else
			{
				std::cerr << "Need NAME but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == PIN_NAME)
		{
			if (t.str == ",")
			{
				if (!pin_check_rules(wires, pin))
				{
					return false;
				}
				comp.pins.push_back(pin);
				state = PINS;
			}
			else if (t.str == ")")
			{
				if (!pin_check_rules(wires,pin))
				{
				return false;
				}
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else if (t.str == "[")
			{
				state = BUS;
			}
			else
			{
				std::cerr << "Need ',' or ')' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER)
			{

				pin.bus_msb = atoi(t.str.c_str());
				state = BUS_MSB;
			}
			else
			{
				std::cerr << "Need NUMBER but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_MSB)
		{
			if (t.str == ":")
			{
				state = BUS_COLON;
			}
			else if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ':' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_COLON)
		{
			if (t.type == evl_token::NUMBER)
			{
				pin.bus_lsb = atoi(t.str.c_str());
				state = BUS_LSB;
			}
			else
			{
				std::cerr << "Need '0' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_LSB)
		{
			if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "Need ']' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_DONE)
		{
			if (t.str == ",")
			{
				if (!pin_check_rules(wires,pin))
				{
				return false;
				}
				comp.pins.push_back(pin);
				state = PINS;
			}
			else if (t.str == ")")
			{
				if (!pin_check_rules(wires,pin))
				{
				return false;
				}
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else
			{
				std::cerr << "Wire'" << t.str << "'on line" << t.line_no << "is already defined" << std::endl;
				return false;
			}
		}
		else if (state == PINS_DONE)
		{
			if (t.str == ";")
			{
				comps.push_back(comp);
				state = DONE;
			}
			else
			{
				std::cerr << "Need ';' but found'" << t.str << "'on line" << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == DONE)
		{
			if (!s.tokens.empty() || (state != DONE))
			{
				std::cerr << "Something wrong with the statement" << std::endl;
				return false;
			}

		}

	}
	return true;
}

//显示wires
void display_wires(std::ostream &out, const evl_wires &wires)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		out << " wire " << it->name << " " << it->width << std::endl;
	}
}

//显示component
void display_comp(std::ostream &out, evl_comps &comps)
{


	for (evl_comps::iterator it = comps.begin(); it != comps.end(); ++it)
	{
		out << "  component " << it->type << it->name << " " << it->pins.size() << std::endl;
		for (evl_pins::iterator it_pin = it->pins.begin(); it_pin != it->pins.end(); ++it_pin)
		{

			out << "    pin " << it_pin->name << it_pin->bus_msb << it_pin->bus_lsb << std::endl;
		}

	}

}

//将tokens并statements中
bool gather_tokens_into_statements(evl_statements &statements, evl_tokens &tokens, evl_wires &wires, std::string & file_name, evl_comps &comps, std::string &module_name)
{
	assert(statements.empty());
	file_name += ".syntax";
	std::ofstream out(file_name.c_str());
	for (; !tokens.empty();)    //precondtion: the container is not empty
	{
		evl_token token = tokens.front();
		if (token.type != evl_token::NAME)
		{
			std::cerr << "Need a NAME token but found '" << token.str << "'on line" << token.line_no << std::endl;
			return false;
		}
		if (token.str == "module")//MODULE statement
		{
			evl_statement module;
			module.type = evl_statement::MODULE;
			if (!push_tokens_to_statemnet(module.tokens, tokens))
				return false;
			statements.push_back(module);
			evl_tokens::iterator it = module.tokens.begin();
			it++;
			out << "module " << it->str;
			module_name = it->str;

		}
		else if (token.str == "wire")
		{
			//WIRE statement
			evl_statement wire;
			wire.type = evl_statement::WIRE;
			if (!push_tokens_to_statemnet(wire.tokens, tokens))
				return false;
			statements.push_back(wire);
			process_wire_statement(wires, wire);//call process_wire_function to process wires in the statement
		}
		else if (token.str == "endmodule")
		{
			//ENDMODULE statement
			evl_statement endmodule;

			endmodule.type = evl_statement::ENDMODULE;
			endmodule.tokens.push_back(token);
			tokens.erase(tokens.begin());
			statements.push_back(endmodule);
		}
		else {
			evl_statement comp;
			comp.type = evl_statement::COMPONENT;
			if (!push_tokens_to_statemnet(comp.tokens, tokens))
				return false;
			statements.push_back(comp);
			process_comp_statement(comps, comp, wires);//call process_comp_function to process wires in the statement
		}
		//else continue;
	}
	//out << " " << wires.size() << " " << comps.size() << std::endl;
	out << "wires " << wires.size() << std::endl;
	display_wires(out, wires);
	out << "components " << comps.size() << std::endl;
	display_comp(out, comps);
	out.close();
	return true;
}

//引用以前的代码
bool parse_evl_file(std::string evl_file, std::string &module_name, evl_wires &wires, evl_comps &comps)
{
	evl_tokens tokens;
	evl_statements statements;
	if (!extract_tokens_from_file(evl_file, tokens))
	{
		std::cerr << "extract_tokens_from_file() fail" << std::endl;
		return false;
	}
	store_tokens_to_file(evl_file, tokens);
	gather_tokens_into_statements(statements, tokens, wires, evl_file, comps, module_name);
	return true;
}

//创建wires_table
evl_wires_table make_wires_table(const evl_wires &wires) {
	evl_wires_table wires_table;
	for (evl_wires::const_iterator it = wires.begin();
		it != wires.end(); ++it) {
		wires_table.insert(std::make_pair(it->name, it->width));
	}
	return wires_table;
}
