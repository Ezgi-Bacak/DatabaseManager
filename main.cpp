#include "DatabaseManager.h"

int main() {
    std::cout << "=== UNIVERSITY DATABASE PROGRAM ===" << std::endl;
    std::cout << "Starting..." << std::endl;

    DatabaseManager db;

    try {
        std::cout << "\n--- Connection Phase ---" << std::endl;

        if (!db.connect()) {
            std::cout << "Program is terminating." << std::endl;
            return -1;
        }

        db.listAllStudents();

        db.searchStudentsByDepartment();

        db.addNewStudents();

        std::cout << "\n--- Updated Student List ---" << std::endl;
        db.listAllStudents();

        std::cout << "\n>>> All operations completed successfully!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    db.disconnect();

    std::cout << "\nProgram finished. Press any key to exit...";
    std::cin.ignore();   
    std::cin.get();    
    return 0;
}
