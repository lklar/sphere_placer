#pragma once
#include <stdexcept>
#include <iostream>
#include <nlohmann/json.hpp>

class PlacerConfig {
public:
	enum mode {
		PP_MONOSIZED = 0,
		PP_POLYSIZED,
		PP_RECURSIVE,
		PP_ANGLES,
		PP_DYNAMICFF
	};
private:
	struct cliParser;

	nlohmann::json config = {
		{"mono_sized", true},
		{"poly_sized", false},
		{"recursive", true},
		{"N", 1000},
		{"R", 0.5},
		{"R_min", 0.0},
		{"R_max", 0.0},
		{"X_dim", 10.0},
		{"Y_dim", 10.0},
		{"Z_floor", 0.0},
		{"extra_theta", 180.0},
		{"N_phi", 1},
		{"output_path", "out.csv"}
	};
	uint32_t flags = 0;
public:
	PlacerConfig();

	PlacerConfig(std::string fName);

	PlacerConfig(int argc, char** argv);

	bool get_flag(mode flag);

	template <typename T>
	auto& operator[](T* key)
	{
		return config[key];
	}

private:
	template <typename T>
	bool set_from_cli(cliParser& cliP, std::string cliKey, std::vector<std::string> jsonKeys)
	{
		if (cliP.data.contains(cliKey))
		{
			std::vector<T> data = cliP.parse<T>(cliKey);
			for (int i = 0; i < jsonKeys.size(); ++i)
			{
				try
				{
					config[jsonKeys[i]] = data[i];
				}
				catch (const std::exception e)
				{
					std::printf("Error: command line option %s expects %i arguments, but only %i were given\n", cliKey, jsonKeys.size(), data.size());
				}
			}
			return true;
		}
		else {
			return false;
		}
	}

	void set_flags();

	void set_flag(mode flag);

	void unset_flag(mode flag);

	struct cliParser
	{
	public:
		std::map<std::string, std::vector<std::string>> data;

		cliParser(int argc, char** argv)
		{
			for (int i = 1; i < argc;)
			{
				std::string key(argv[i]);
				std::vector<std::string> values;
				++i;
				while (i < argc && argv[i][0] != '-')
				{
					values.emplace_back(argv[i]);
					++i;
				}

				data[key] = values;
			}
		}

		template <typename T>
		T parseSingle(std::string s)
		{
			T t{};
			return T;
		}

		template<>
		int parseSingle<int>(std::string s)
		{
			return std::stoi(s);
		}

		template<>
		double parseSingle<double>(std::string s)
		{
			return std::stod(s);
		}

		template<>
		bool parseSingle<bool>(std::string s)
		{
			return std::stoi(s) != 0;
		}

		template<>
		std::string parseSingle<std::string>(std::string s)
		{
			return s;
		}

		template <typename T>
		std::vector<T> parse(std::string key)
		{
			std::vector<T> result;
			for (auto& elem : data[key])
			{
				try
				{
					T t = parseSingle<T>(elem);
					result.emplace_back(t);
				}
				catch (const std::exception e)
				{
					std::cout << "Error parsing \"" << elem << "\": incorrect type\n";
				}
			}
			return result;
		}
	};
};
