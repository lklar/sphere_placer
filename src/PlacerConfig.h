#pragma once
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include "Raster.h"
#include <random>
#include "RNG/RNG.h"

class PlacerConfig {
private:
	struct CliParser;

public:
	int n = 1000;
	double x_dim = 10;
	double y_dim = 10;
	double r = 0.5;
	double r_min = 0.5;
	double r_max = 0.5;
	double theta_a = 180;
	int n_phi = 1;
	std::string output_path = "out.csv";
	bool recursive_mode = true;
	std::function<double(RNG::xoshiro256ss&)> radius_generator = 
		[&r_min = this->r_min, &r_max = this->r_max](RNG::xoshiro256ss& rng) {
			static std::uniform_real_distribution<double> dist(r_min, r_max);
			return dist(rng);
	};


	PlacerConfig() {};

	PlacerConfig(int argc, char** argv);

private:

	struct CliParser
	{
	public:
		std::map<std::string, std::vector<std::string>> data;

		CliParser(int argc, char** argv)
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

		template <typename T>
		bool set_from_cli(std::string cliKey, std::vector<std::reference_wrapper<T>> values)
		{
			if (data.contains(cliKey))
			{
				std::vector<T> parsedValues = parse<T>(cliKey);
				if (values.size() != parsedValues.size())
				{
					std::printf("Error: command line option %s expects %i arguments, but %i were given\n", cliKey, values.size(), parsedValues.size());
				}
				for (int i = 0; i < values.size() && i < parsedValues.size(); ++i)
				{
					values[i].get() = parsedValues[i];
				}
				return true;
			}
			else {
				return false;
			}
		}
	};
};
