#include "database.hpp"
#include <string>

using namespace std;

Database::Database(const string &dbName){
    if(sqlite3_open(dbName.c_str(), &db)){
        cerr << "Can't open database" << sqlite3_errmsg(db) << endl;
    }
}

Database::~Database(){
    sqlite3_close(db);
}

void Database::init(){
    // Initialize needed Table
    char *errMsg = nullptr;

    // Enabling Foreign Key support on SQLite
    const char *foreignKeySupport = "PRAGMA foreign_keys = ON;";
    int execute_sql = sqlite3_exec(db, foreignKeySupport, 0, 0, &errMsg);
    
    if (execute_sql!= SQLITE_OK) {
        cerr << "Error enabling foreign keys: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Category Table
    const char *categoryTableQuery = "CREATE TABLE IF NOT EXISTS category ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                     "name TEXT NOT NULL, "
                                     "description TEXT NOT NULL);";
    

    execute_sql = sqlite3_exec(db, categoryTableQuery, nullptr, nullptr, &errMsg);
    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating Category Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    // Supplier Table
    const char *supplierTableQuery = "CREATE TABLE IF NOT EXISTS suppliers ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                     "name TEXT NOT NULL, "
                                     "address TEXT NOT NULL, "
                                     "phone TEXT, "
                                     "email TEXT);";
    
    execute_sql = sqlite3_exec(db, supplierTableQuery, nullptr, nullptr, &errMsg);
    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating Supplier Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }


    // Items Table
    const char *itemTableQuery = "CREATE TABLE IF NOT EXISTS item ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "name TEXT NOT NULL, "
                                 "description TEXT NOT NULL, "
                                 "category_id INTEGRE NOT NULL, "
                                 "quantity INTEGER NOT NULL, "
                                 "unit_measurement TEXT NOT NULL, "
                                 "unit_price REAL NOT NULL, "
                                 "price REAL NOT NULL, "
                                 "supplier_id INTEGER NOT NULL, "
                                 "FOREIGN KEY(category_id) REFERENCES category(id), "
                                 "FOREIGN KEY(supplier_id) REFERENCES suppliers(id)"
                                 ");";
    execute_sql = sqlite3_exec(db, itemTableQuery, nullptr, nullptr, &errMsg);
    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating Items Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }    

    const char *userTableQuery = "CREATE TABLE IF NOT EXISTS user ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "username TEXT NOT NULL, "
                                 "password TEXT NOT NULL, "
                                 "role TEXT NOT NULL, "
                                 "contact_info TEXT NOT NULL"
                                 ");";

    execute_sql = sqlite3_exec(db, userTableQuery, nullptr, nullptr, &errMsg);
    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating User Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    const char *transactionTableQuery = "CREATE TABLE IF NOT EXISTS transaction_records ("
                                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                        "item_id INTEGER NOT NULL, "
                                        "transaction_type TEXT NOT NULL, "
                                        "quantity INTEGER NOT NULL, "
                                        "transaction_date TEXT NOT NULL, "
                                        "user_id INTEGER NOT NULL, "
                                        "remarks TEXT, "
                                        "FOREIGN KEY(item_id) REFERENCES item(id), "
                                        "FOREIGN KEY(user_id) REFERENCES user(id)"
                                        ");";
    execute_sql = sqlite3_exec(db, transactionTableQuery, nullptr, nullptr, &errMsg);
    if (execute_sql != SQLITE_OK) {
        cerr << "Error Creating Transaction Table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

sqlite3 *Database::getDBConnection() const{
    return db;
}