#include "simulation.h"

bool and_gate::validate_structural_semantics()
{
	if (pins_.size() < 3)
	{
		return false;
	}
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i)
		pins_[i]->set_as_input();
	return true;
}

bool and_gate::compute()
{
	bool output = pins_[1]->get_value();
	for (int i = 2; i != pins_.size(); ++i)
	{
		output &= pins_[i]->get_value();
	}

	if (output == 1) return true;
	else
	{
		return false;
	}
}

bool or_gate::validate_structural_semantics()
{
	if (pins_.size() < 3)
	{
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		if ((*pins_[i]).nets_.size() != 1)
		{
			return false;
		}
		if (i == 0) pins_[i]->set_as_output();
		else pins_[i]->set_as_input();
	}
	return true;
}

bool or_gate::compute()
{
	bool output = pins_[1]->get_value();
	for (int i = 2; i != pins_.size(); ++i)
	{
		output |= pins_[i]->get_value();
	}
	if (output == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool xor_gate::validate_structural_semantics()
{
	if (pins_.size() < 3)
	{
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		if ((*pins_[i]).nets_.size() != 1)
		{
			return false;
		}
		if (i == 0) pins_[i]->set_as_output();
		else pins_[i]->set_as_input();
	}
	return true;
}

bool xor_gate::compute()
{
	bool output = pins_[1]->get_value();
	for (int i = 2; i != pins_.size(); ++i)
	{
		output ^= pins_[i]->get_value();
	}
	return output;
}

bool not_gate::validate_structural_semantics()
{
	if (pins_.size() != 2)
	{
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		if ((*pins_[i]).nets_.size() != 1)
		{
			return false;
		}
		if (i == 0) pins_[i]->set_as_output();
		else pins_[i]->set_as_input();
	}
	return true;
}

bool not_gate::compute()
{
	if (pins_[1]->get_value() == 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool buf_gate::validate_structural_semantics()
{
	if (pins_.size() != 2)
	{
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		if ((*pins_[i]).nets_.size() != 1)
		{
			return false;
		}
		if (i == 0)
		{
			pins_[i]->set_as_output();
		}
		else
		{
			pins_[i]->set_as_input();
		}
	}
	return true;
}

bool buf_gate::compute()
{
	if (pins_[1]->get_value() == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool evl_flip_flop::validate_structural_semantics()
{
	if (pins_.size() != 3)
	{
		return false;
	}
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	pins_[2]->set_as_input();
	return true;
}

bool evl_flip_flop::compute()
{
	net *input_net = pins_[1]->get_net();
	next_state_ = input_net->retrieve_logic_value();
	return next_state_;
}

bool evl_one_gate::validate_structural_semantics()
{
	if (pins_.size() < 1) {
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		pins_[i]->set_as_output();
		for (int j = 0; j < pins_[i]->nets_.size(); ++j)
		{
			pins_[i]->nets_[j]->value_ = 1;
		}
	}
	return true;
}

bool evl_one_gate::compute()
{
	return true;
}

bool evl_zero_gate::validate_structural_semantics()
{
	if (pins_.size() < 1) {

		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		pins_[i]->set_as_output();
		for (int j = 0; j < pins_[i]->nets_.size(); ++j)
		{
			pins_[i]->nets_[j]->value_ = 0;
		}
	}
	return true;
}

bool evl_zero_gate::compute()
{
	return false;
}

bool evl_input_gate::validate_structural_semantics()
{
	if (pins_.size() < 1) {
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
	{
		pins_[i]->set_as_output();
		return false;
	}
}

bool evl_output_gate::validate_structural_semantics()
{
	if (pins_.size() < 1)
	{
		return false;
	}
	for (int i = 0; i < pins_.size(); ++i)
		pins_[i]->set_as_input();
	return true;
}

bool evl_clock_gate::validate_structural_semantics()
{
	pins_[0]->nets_[0]->value_ = 1;
	return true;
}

bool evl_clock_gate::compute()
{
	return true;
}

bool evl_lut_gate::validate_structural_semantics()
{
	if (pins_.size() != 2)
	{
		return false;
	}
	pins_[0]->set_as_output();
	pins_[1]->set_as_output();
	return true;
}

bool evl_lut_gate::compute()
{
	return false;
}

bool tris_gate::validate_structural_semantics()
{
	if (pins_.size() != 3)
	{
		return false;
	}
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	pins_[2]->set_as_input();
	return true;
}

bool tris_gate::compute()
{
	if (pins_[2]->get_value() == 1)
	{
		return pins_[1]->get_value();
	}
	else
	{
		return true;
	}
}

