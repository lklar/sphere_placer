#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <format>
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
		std::string content;
		{
			ifs.open(filePath, std::ios_base::in);

			ifs.seekg(0, ifs.end);
			size_t fileSize = ifs.tellg();
			ifs.seekg(0, ifs.beg);

			content.resize(fileSize, '\0');
			ifs.read(content.data(), fileSize);
			ifs.close();
		}

		std::vector<T> allData;

		size_t lineStart = 0;
		size_t lineEnd;

		size_t lineNumber = 0;

		while ((lineEnd = content.find('\n', lineStart)) != std::string::npos)
		{
			std::string_view line(content.data() + lineStart, lineEnd - lineStart);

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
				printf("Error while parsing file %s line %d: Not enough arguments\n", filePath, lineNumber);
			}
			lineStart = lineEnd + 1;
			++lineNumber;
		}

		return allData;
	}

	template<typename T>
		requires std::constructible_from<T, double>
	std::vector<T> read_from_csv()
	{
		std::string content;
		{
			ifs.open(filePath, std::ios_base::in);

			ifs.seekg(0, ifs.end);
			size_t fileSize = ifs.tellg();
			ifs.seekg(0, ifs.beg);

			content.resize(fileSize, '\0');
			ifs.read(content.data(), fileSize);
			ifs.close();
		}

		std::vector<T> allData;

		size_t lineStart = 0;
		size_t lineEnd;

		size_t lineNumber = 0;

		while ((lineEnd = content.find('\n', lineStart)) != std::string::npos)
		{
			double result;
			try 
			{
				std::from_chars(content.data() + lineStart, content.data() + lineEnd, result);
				allData.emplace_back(result);
			}
			catch (const std::exception e)
			{
				std::string_view line(content.data() + lineStart, lineEnd - lineStart);
				std::printf("Error parsing file %s in line %d:\n%s\n", filePath, lineNumber, line);
			}
			lineStart = lineEnd + 1;
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