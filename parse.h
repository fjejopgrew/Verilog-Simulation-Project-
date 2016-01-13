#ifndef _PARSE_H
#define _PARSE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <list>
#include <map>

//struct evl_token 定义evl_token
struct evl_token
{
	enum token_type { NAME, NUMBER, SINGLE };
	token_type type;
	std::string str;
	int line_no;
}; 
typedef std::list<evl_token> evl_tokens;

//struct evl_statement 定义evl_statement
struct evl_statement
{
	enum statement_type{ MODULE, WIRE, COMPONENT, ENDMODULE };
	statement_type type;
	evl_tokens tokens;
};
typedef std::list<evl_statement>evl_statements;

//定义evl_wire
struct evl_wire
{
	std::string name;
	int width;
};
typedef std::list<evl_wire> evl_wires;
typedef std::map<std::string, int> evl_wires_table;//定义map evl_wires_table
evl_wires_table make_wires_table(const evl_wires &wires);//定义make_wires_table

//struct evl_pin 定义evl_pin
struct evl_pin
{
	std::string name;
	int bus_msb;
	int bus_lsb;
};
typedef std::list<evl_pin> evl_pins;

//struct evl_comp 定义evl_comp
struct evl_comp
{
	std::string name;
	std::string type;
	evl_pins pins;
};
typedef std::list<evl_comp>evl_comps;

//引用以前的代码
bool parse_evl_file(std::string, std::string &, evl_wires &, evl_comps &);
#endif
