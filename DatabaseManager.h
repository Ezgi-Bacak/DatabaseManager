#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>

class DatabaseManager {
private:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;

public:
	DatabaseManager();
	~DatabaseManager();

	bool connect();
	void disconnect();
	void listAllStudents();
	void searchStudentsByDepartment();
	void addNewStudents();
};