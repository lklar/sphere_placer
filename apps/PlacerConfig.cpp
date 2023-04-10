#pragma once
#include "PlacerConfig.h"
#include <fstream>

PlacerConfig::PlacerConfig()
{
	set_flags();
};

PlacerConfig::PlacerConfig(std::string fName)
{
	nlohmann::json buffer;
	std::ifstream ifs(fName);
	ifs >> buffer;
	for (auto it = buffer.begin(); it != buffer.end(); ++it)
	{
		config[it.key()] = it.value();
	}
	set_flags();
}

PlacerConfig::PlacerConfig(int argc, char** argv)
{
	cliParser cliP(argc, argv);
	if (cliP.data.contains("-f"))
	{
		std::string configFileName = cliP.parse<std::string>("-f").front();
		std::ifstream ifs(configFileName);
		nlohmann::json buffer;
		ifs >> buffer;
		for (auto it = buffer.begin(); it != buffer.end(); ++it)
		{
			config[it.key()] = it.value();
		}
	}

	set_from_cli<int>(cliP, "-n", { "N" });
	set_from_cli<double>(cliP, "-r", { "R" });
	set_from_cli<double>(cliP, "-rvar", { "R_min", "R_max" });
	set_from_cli<double>(cliP, "-dims", { "X_dim", "Y_dim" });
	set_from_cli<double>(cliP, "-theta", { "extra_theta" });
	set_from_cli<int>(cliP, "-phi", { "N_phi" });
	set_from_cli<std::string>(cliP, "-o", { "output_path" });
	set_from_cli<bool>(cliP, "-mono", { "mono_sized" });
	set_from_cli<bool>(cliP, "-poly", { "poly_sized" });
	set_from_cli<bool>(cliP, "-recursive", { "recursive_mode" });

	set_flags();
}

bool PlacerConfig::get_flag(mode flag)
{
	bool result = flags & (1u << flag);
	return result;
}

void PlacerConfig::set_flags()
{
	if (config["mono_sized"])
	{
		set_flag(mode::PP_MONOSIZED);
	}
	if (config["poly_sized"])
	{
		set_flag(mode::PP_POLYSIZED);
		set_flag(mode::PP_POLYSIZED);
	}
	if (config["recursive"])
	{
		set_flag(mode::PP_RECURSIVE);
	}
}

void PlacerConfig::set_flag(mode flag)
{
	flags |= (1u << flag);
}

void PlacerConfig::unset_flag(mode flag)
{
	flags &= ~(1u << flag);
}