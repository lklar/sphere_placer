#include "file_interface.h"
#include <unordered_map>
#include <functional>
#include <format>

// Database
sqlite3Interface::sqlite3Interface()
{
	sqlite3_open(":memory:", &database);
}

sqlite3Interface::~sqlite3Interface()
{
	sqlite3_close_v2(database);
}

int sqlite3Interface::load(std::filesystem::path fPath)
{
	int rc;
	sqlite3* dbFile;
	sqlite3_backup* pBackup;

	rc = sqlite3_open(fPath.string().c_str(), &dbFile);
	if (rc == SQLITE_OK) {

		pBackup = sqlite3_backup_init(database, "main", dbFile, "main");
		if (pBackup) {
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(database);
	}
	sqlite3_close_v2(dbFile);
	return rc;
}

int sqlite3Interface::load_from_drive(std::filesystem::path fPath)
{
	return sqlite3_open(fPath.string().c_str(), &database);
}

int sqlite3Interface::save(std::filesystem::path fPath)
{
	int rc;
	sqlite3* dbFile;
	sqlite3_backup* pBackup;

	rc = sqlite3_open(fPath.string().c_str(), &dbFile);
	if (rc == SQLITE_OK) {

		pBackup = sqlite3_backup_init(dbFile, "main", database, "main");
		if (pBackup) {
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(dbFile);
	}
	sqlite3_close_v2(database);
	return rc;
}

void sqlite3Interface::close()
{
	sqlite3_close_v2(database);
}

std::vector<double3> sqlite3Interface::read_positions(std::string tableName, std::string conditions)
{
	std::vector<double3> data;
	sqlite3_stmt* stmt;
	std::string query = std::format("select xPos, yPos, zPos from '{0}' {1}", tableName, conditions);
	sqlite3_prepare(database, query.c_str(), -1, &stmt, NULL);

	int rc;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		data.emplace_back(
			sqlite3_column_double(stmt, 0),
			sqlite3_column_double(stmt, 1),
			sqlite3_column_double(stmt, 2)
		);
	}
	data.shrink_to_fit();
	return data;
}

int sqlite3Interface::read_doubles(std::string query, std::vector<std::vector<double>>& data)
{
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, query.c_str(), -1, &stmt, NULL);
	int rc;

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			double buffer = sqlite3_column_double(stmt, i);
			data[i].emplace_back(buffer);
		}
	}
	data.shrink_to_fit();
	return rc;
}

std::map<std::string, double> sqlite3Interface::read_parameters()
{
	std::map<std::string, double> parameters;
	sqlite3_stmt* stmt;
	std::string szSQL = "select * from Parameters";
	sqlite3_prepare(database, szSQL.c_str(), -1, &stmt, NULL);
	int rc;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		std::string name;
		std::stringstream ss;
		ss << sqlite3_column_text(stmt, 0);
		ss >> name;
		parameters[name] = sqlite3_column_double(stmt, 1);
	}
	return parameters;
}


std::vector<std::string> sqlite3Interface::get_table_names()
{
	int rc;
	sqlite3_stmt* szSQL;
	std::vector<std::string> table_names;
	sqlite3_prepare(database, "select name from sqlite_master where type='table' order by name", -1, &szSQL, NULL);

	while ((rc = sqlite3_step(szSQL)) == SQLITE_ROW)
	{
		std::string name;
		std::stringstream ss;
		ss << sqlite3_column_text(szSQL, 0);
		ss >> name;
		table_names.emplace_back(name);
	}
	table_names.shrink_to_fit();
	return table_names;
}

long long sqlite3Interface::get_row_count(std::string tableName)
{
	sqlite3_stmt* stmt;
	std::string query = std::format("select count(rowid) from '{0}'", tableName);
	sqlite3_prepare(database, query.c_str(), -1, &stmt, NULL);
	int rc;

	long long result = 0;

	if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		result = sqlite3_column_int64(stmt, 0);
	}
	return result;
}

void sqlite3Interface::write_doubles(std::string insertQuery, std::vector<std::vector<double>>& data)
{
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, insertQuery.c_str(), strlen(insertQuery.c_str()), &stmt, 0);

	int rowId = 0;
	for (int step = 0; step < data.front().size(); ++step)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			sqlite3_bind_double(stmt, i + 1, data[i][step]);
		}
		sqlite3_step(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_finalize(stmt);
}

void sqlite3Interface::write_row(std::string tableName, std::vector<std::pair<std::string, double>> data)
{
	std::string names = data.front().first;
	std::string questionMarks = "?";
	for (int i = 1; i < data.size(); ++i)
	{
		names.append(std::format(", {0}", data[i].first));
		questionMarks.append(",?");
	}

	sqlite3_stmt* stmt;
	std::string insertQuery = std::format("insert into '{0}' ({1}) values ({2})", tableName, names, questionMarks);
	sqlite3_prepare(database, insertQuery.c_str(), strlen(insertQuery.c_str()), &stmt, 0);

	for (int i = 0; i < data.size(); ++i)
	{
		sqlite3_bind_double(stmt, i + 1, data[i].second);
	}
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void sqlite3Interface::write_positions(std::string tableName, std::vector<double3>& positions)
{
	char* errmsg;
	std::string creationStatement = std::format("create table '{0}' (ID int, xPos real, yPos real, zPos real)", tableName);
	sqlite3_exec(database, creationStatement.c_str(), 0, 0, &errmsg);

	sqlite3_stmt* stmt;
	std::string insertQuery = std::format("insert into '{0}' (ID, xPos, yPos, zPos) values (?,?,?,?)", tableName).c_str();
	sqlite3_prepare(database, insertQuery.c_str(), strlen(insertQuery.c_str()), &stmt, 0);

	int rowId = 0;
	for (auto& pos : positions)
	{
		sqlite3_bind_int(stmt, 1, rowId++);
		sqlite3_bind_double(stmt, 2, pos.x);
		sqlite3_bind_double(stmt, 3, pos.y);
		sqlite3_bind_double(stmt, 4, pos.z);

		sqlite3_step(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_finalize(stmt);
}

void sqlite3Interface::write_pos_vel(std::string tableName, std::vector<double3>& positions, double3 velocity)
{
	char* errmsg;
	std::string creationStatement = std::format("create table '{0}' (ID int, xPos real, yPos real, zPos real, xVel real, yVel real, zVel real)", tableName);
	sqlite3_exec(database, creationStatement.c_str(), 0, 0, &errmsg);


	sqlite3_stmt* stmt;
	std::string insertQuery = std::format("insert into '{0}' (ID, xPos, yPos, zPos, xVel, yVel, zVel) values (?,?,?,?,?,?,?)", tableName).c_str();
	sqlite3_prepare(database, insertQuery.c_str(), strlen(insertQuery.c_str()), &stmt, 0);

	int rowId = 0;
	for (auto& pos : positions)
	{
		sqlite3_bind_int(stmt, 1, rowId++);
		sqlite3_bind_double(stmt, 2, pos.x);
		sqlite3_bind_double(stmt, 3, pos.y);
		sqlite3_bind_double(stmt, 4, pos.z);
		sqlite3_bind_double(stmt, 5, velocity.x);
		sqlite3_bind_double(stmt, 6, velocity.y);
		sqlite3_bind_double(stmt, 7, velocity.z);

		sqlite3_step(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_finalize(stmt);
}
	
void sqlite3Interface::write_parameters(std::map<std::string, double>& parameters)
{
	std::string tableStmt =
		"create table 'Parameters' (Name text, Value real)";

	char* errmsg;
	sqlite3_exec(database, tableStmt.c_str(), 0, 0, &errmsg);

	sqlite3_stmt* stmt;
	std::string szSQL = "insert into Parameters (Name, Value) values (?,?)";
	sqlite3_prepare(database, szSQL.c_str(), strlen(szSQL.c_str()), &stmt, 0);
	for (auto& param : parameters)
	{
		sqlite3_bind_text(stmt, 1, param.first.c_str(), param.first.size(), 0);
		sqlite3_bind_double(stmt, 2, param.second);
		sqlite3_step(stmt);
		sqlite3_reset(stmt);
	}
}


void sqlite3Interface::execute_statement(std::string statement)
{
	char* errmsg;
	sqlite3_exec(database, statement.c_str(), 0, 0, &errmsg);
}
