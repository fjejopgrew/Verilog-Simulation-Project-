#include <iostream>
#include <fstream>
#include "netlist.h"
#include "parse.h"



int main(int argc, char *argv[])
{
	evl_wires wires;
	evl_comps comps;
	std::string module_name;
	//argv[1] = "C:\\Users\\gaohang\\Desktop\\ECE 449\\449李超\\golden\\counter.evl";
	//argv[1] = "C:\\Users\\gaohang\\Desktop\\ECE 449\\WinSCP\\hgao\\golden\\io.evl";
	//argv[1] = "C:\\Users\\gaohang\\Desktop\\ECE 449\\WinSCP\\hgao\\golden\\simple_seq.evl";
	argv[1] = "C:\\Users\\gaohang\\Desktop\\ECE 449\\WinSCP\\hgao\\golden\\bus.evl";
	//argv[1] = "C:\\Users\\gaohang\\Desktop\\ECE 449\\WinSCP\\hgao\\golden\\tris_lut.evl";
	argc = 3;
	if (argc < 2)
	{
		std::cerr << "You should provide a file name." << std::endl;
		return -1;
	}
	std::ifstream input_file(argv[1]);
	if (!input_file)
	{
		std::cerr << "I can't read " << argv[1] << "." << std::endl;
		return -1;
	}
	std::string file_name(argv[1]);

	if (!parse_evl_file(argv[1], module_name, wires, comps))
		return -1;
	evl_wires_table wires_table = make_wires_table(wires);
	netlist nl;
        if (!nl.create(wires, comps, wires_table))
		return -1;
	std::string nl_file = std::string(argv[1]) + ".netlist";
	std::ofstream out(nl_file.c_str());
	nl.display(nl_file, module_name, out);
	nl.simulation(1000, file_name);
	return 0;
}



