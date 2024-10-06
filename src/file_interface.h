#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <format>
#include <sstream>
#include "dim_N_vectors/dim_3_vectors.h"
#include <filesystem>

class FileInterface {
public:
	enum mode{
		FI_BINARY = 0,
		FI_CSV
	};
private:
	std::ifstream ifs;
	std::ofstream ofs;
	std::filesystem::path filePath;
	mode fMode;

public:
	FileInterface(
		std::string fPath,
		mode fMode
	) :
		filePath(fPath),
		fMode(fMode) {};

	~FileInterface()
	{
		if (ifs.is_open())
		{
			ifs.close();
		}
		if (ofs.is_open())
		{
			ofs.close();
		}
	}

	template<typename T>
	std::vector<T> read()
	{
		switch(fMode)
		{
		case mode::FI_BINARY:
			return read_from_binary<T>();
			break;

		case mode::FI_CSV:
			return read_from_csv<T>();
			break;
		}
	}

	template<typename T>
	void write(std::vector<T>& data)
	{
		switch (fMode)
		{
		case mode::FI_BINARY:
			write_to_binary<T>(data);
			break;

		case mode::FI_CSV:
			write_to_csv<T>(data);
			break;
		}
	}

private:
	template<typename T>
	std::vector<T> read_from_binary()
	{
		ifs.open(filePath, std::ios_base::binary | std::ios_base::in);
		std::vector<T> vec;
		double buffer;
		size_t numberOfSpheres;
		ifs.peek();
		if (ifs.good())
		{
			ifs.read(reinterpret_cast<char*>(&numberOfSpheres), sizeof(size_t));
			vec.resize(numberOfSpheres);
			ifs.read(reinterpret_cast<char*>(vec.data()), numberOfSpheres * sizeof(T));
			ifs.peek();
		}
		ifs.close();
		return vec;
	}

	template<typename T>
	void write_to_binary(std::vector<T>& data, std::ios_base::openmode openmode = static_cast<std::ios_base::openmode>(0))
	{
		ofs.open(filePath, std::ios::out | std::ios::binary | openmode);
		size_t numberOfEntries = data.size();
		ofs.write(reinterpret_cast<const char*>(&numberOfEntries), sizeof(numberOfEntries));
		ofs.write(reinterpret_cast<const char*>(data.data()), numberOfEntries * sizeof(T));
		ofs.close();
	}

	template<typename T>
		requires std::constructible_from<T, std::vector<double>&>
	std::vector<T> read_from_csv()
	{
		std::vector<T> allData;

		std::vector<T> allData;

		std::stringstream iss;
		ifs.open(filePath, std::ios_base::in);
		iss << ifs.rdbuf();

		std::string line;

		size_t lineNumber = 0;

		while (std::getline(iss, line, '\n'))
		{

			std::vector<double> fields;
			auto lEnd = line.data() + line.size();
			fields.emplace_back();
			for (auto fc_result = std::from_chars(line.data(), lEnd, fields.back()); fc_result.ptr < lEnd;)
			{
				fields.emplace_back();
				fc_result = std::from_chars(fc_result.ptr + 1, lEnd, fields.back());
			}

			try
			{
				allData.emplace_back(fields);
			}
			catch(std::exception e)
			{
				std::cout << "Error while parsing file " << filePath <<  " line " << lineNumber << ": Not enough valid arguments\n";
			}
			++lineNumber;
		}

		return allData;
	}

	template<typename T>
		requires std::constructible_from<T, double>
	std::vector<T> read_from_csv()
	{
		std::vector<T> allData;

		std::stringstream iss;
		ifs.open(filePath, std::ios_base::in);
		iss << ifs.rdbuf();

		std::string line;
		int lineNumber = 0;
		while (std::getline(iss, line, '\n'))
		{
			double result;
			auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), result);
			if (ec == std::errc() && ptr == line.data() + line.size())
			{
				allData.emplace_back(result);
			}
			else {
				std::cout << "Error parsing file " << filePath << " in line " << lineNumber << ": " << line << "\n";
			}
			++lineNumber;
		}

		return allData;
	}

	template <typename T>
		requires std::is_convertible<T, std::vector<double>>::value
	void write_to_csv(std::vector<T>& data)
	{
		std::ofstream ofs(filePath, std::ios_base::out);

		for (auto& elem : data)
		{
			std::vector<double> vec = elem;
			ofs << std::format("{:.5f}", vec[0]);
			for (int col = 1; col < vec.size(); ++col)
			{
				ofs << "," << std::format("{:.5f}", vec[col]);
			}
			ofs << "\n";
		}

		ofs.close();
	}
};

void write_to_csv(std::filesystem::path filePath, std::vector<double3>& positions);



template <typename T>
void write_to_csv_transverse(std::filesystem::path filePath, std::vector<std::vector<T>>& data)
{
	std::ofstream ofs(filePath, std::ios_base::out);

	for (int row = 0; row < data.size(); ++row)
	{
		ofs << data[row][0];
		for (int col = 1; col < data.front().size(); ++col)
		{
			ofs << "," << data[row][col];
		}
		ofs << "\n";
	}

	ofs.close();
}