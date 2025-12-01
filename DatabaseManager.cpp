#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() : henv(NULL), hdbc(NULL), hstmt(NULL) {}

DatabaseManager::~DatabaseManager() { disconnect();}

bool DatabaseManager::connect() {
    SQLRETURN retcode;

    std::cout << "1. Preparing environment..." << std::endl;
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "ERROR: Failed to allocate environment handle!" << std::endl;
        return false;
    }

    std::cout << "2. Setting ODBC version..." << std::endl;
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "ERROR: Failed to set ODBC version!" << std::endl;
        return false;
    }

    std::cout << "3. Preparing database connection..." << std::endl;
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "ERROR: Failed to allocate connection handle!" << std::endl;
        return false;
    }

    std::cout << "4. Connecting to SQL Server..." << std::endl;

    SQLWCHAR* connectionString =
        (SQLWCHAR*)L"DRIVER={ODBC Driver 17 for SQL Server};SERVER=DESKTOP-OE76DK0;DATABASE=university;Trusted_Connection=yes;Encrypt=no;TrustServerCertificate=yes;";

    retcode = SQLDriverConnectW(hdbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "ERROR: Failed to connect to SQL Server!" << std::endl;
        std::wcout << L"Please check your connection string: " << connectionString << std::endl;
        return false;
    }

    std::cout << "5. Preparing SQL statement..." << std::endl;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "ERROR: Failed to allocate statement handle!" << std::endl;
        return false;
    }

    std::cout << ">>> Successfully connected to Microsoft SQL Server!" << std::endl;
    return true;

}
void DatabaseManager :: disconnect() {
    std::cout << "Closing connection..." << std::endl;

    if (hstmt) {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = NULL;
    }

    if (hdbc) {
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        hdbc = NULL;
    }

    if (henv) {
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = NULL;
    }

    std::cout << "Connection closed safely." << std::endl;
}
void DatabaseManager::listAllStudents() {
    SQLRETURN retcode;

    std::cout << "\n=== LISTING ALL STUDENTS ===" << std::endl;

    SQLWCHAR query[] = L"SELECT ID, name, dept_name, tot_cred FROM student;";

    retcode = SQLExecDirectW(hstmt, query, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

        char studentId[50];
        char name[200];
        char department[200];
        int credits;

        SQLLEN ind1, ind2, ind3, ind4;

        std::cout << "ID\t\tName\t\tDepartment\t\tCredits\n";
        std::cout << "------------------------------------------------------------------\n";

        while (SQLFetch(hstmt) == SQL_SUCCESS) {

            SQLGetData(hstmt, 1, SQL_C_CHAR, studentId, sizeof(studentId), &ind1);
            SQLGetData(hstmt, 2, SQL_C_CHAR, name, sizeof(name), &ind2);
            SQLGetData(hstmt, 3, SQL_C_CHAR, department, sizeof(department), &ind3);
            SQLGetData(hstmt, 4, SQL_C_SLONG, &credits, 0, &ind4);

            std::cout << studentId << "\t"
                << name << "\t"
                << department << "\t"
                << credits << std::endl;
        }
    }
    else {
        std::cout << "ERROR: Query failed!" << std::endl;
    }

    SQLCloseCursor(hstmt);
}


void DatabaseManager::addNewStudents() {
    SQLRETURN retcode;

    wchar_t id[10];
    wchar_t name[100];
    wchar_t department[50];
    int totalCredits;

    std::cout << "\n=== ADD NEW STUDENT ===" << std::endl;

    std::wcout << L"Student ID (9 digits): ";
    std::wcin.ignore();
    std::wcin.getline(id, 10);

    std::wcout << L"Student name: ";
    std::wcin.getline(name, 100);

    std::wcout << L"Department name: ";
    std::wcin.getline(department, 50);

    std::wcout << L"Total credits: ";
    std::cin >> totalCredits;

    SQLWCHAR query[] =
        L"INSERT INTO student (ID, name, dept_name, tot_cred) VALUES (?, ?, ?, ?)";

    retcode = SQLPrepareW(hstmt, query, SQL_NTS);

 
    retcode = SQLBindParameter(
        hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        9, 0, id, 0, NULL
    );


    retcode = SQLBindParameter(
        hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        100, 0, name, 0, NULL
    );

    retcode = SQLBindParameter(
        hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        50, 0, department, 0, NULL
    );

    retcode = SQLBindParameter(
        hstmt, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
        0, 0, &totalCredits, 0, NULL
    );

    retcode = SQLExecute(hstmt);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        std::wcout << L">>> Student \"" << name << L"\" added successfully!" << std::endl;
    }
    else {
        std::wcout << L">>> Error occurred while adding the student!" << std::endl;

        SQLWCHAR sqlstate[6];
        SQLWCHAR message[256];
        if (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlstate, NULL, message, 256, NULL) == SQL_SUCCESS) {
            std::wcout << L"Error Detail: " << sqlstate << L" - " << message << std::endl;
        }
    }
}
void DatabaseManager::searchStudentsByDepartment() {
    SQLRETURN retcode;
    wchar_t department[50];

    std::cout << "\n=== SEARCH STUDENTS BY DEPARTMENT ===" << std::endl;
    std::cout << "Enter the department you want to search: ";

    std::wcin.getline(department, 50); // <-- Sadece bu

    SQLWCHAR query[] = L"SELECT ID, name FROM student WHERE dept_name = ?";

    retcode = SQLPrepareW(hstmt, query, SQL_NTS);

    SQLBindParameter(
        hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        50, 0, department, 0, NULL
    );

    retcode = SQLExecute(hstmt);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

        SQLWCHAR studentId[20];
        SQLWCHAR name[100];

        std::wcout << L"\nStudents in department \"" << department << L"\":\n";
        std::wcout << L"ID\tName\n";
        std::wcout << L"------------------------" << std::endl;

        bool found = false;

        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            found = true;

            SQLGetData(hstmt, 1, SQL_C_WCHAR, studentId, sizeof(studentId), NULL);
            SQLGetData(hstmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);

            std::wcout << studentId << L"\t" << name << std::endl;
        }

        if (!found) {
            std::wcout << L"No students found in this department." << std::endl;
        }
    }
    else {
        std::cout << "ERROR: Search query execution failed!" << std::endl;
    }

    SQLCloseCursor(hstmt);
}

