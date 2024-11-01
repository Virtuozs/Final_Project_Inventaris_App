#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <sqlite3.h>
#include <functional>
#include <map>
#include <iostream>
#include <variant>

/**
 * @brief Represents a database connection.
 */
class Database {
    private :
        /**
         * @brief Pointer to the SQLite database connection.
         * 
         * This variable holds the connection to the SQLite database,
         * allowing for executing SQL statements and managing the database.
         * It is initialized in the constructor and closed in the destructor.
         */
        sqlite3 *db;

    public :
        /**
         * @brief Constructs a Database object with the specified database name.
         * 
         * This constructor initializes the SQLite database connection using the
         * provided database name. If the database does not exist, it will be created.
         * 
         * @param dbName The name of the database file to be opened or created.
         * If the file extension is omitted, ".db" will be added by default.
         */
        Database(const std::string &dbName);

        /**
         * @brief Destroys the Database object and closes the database connection.
         * 
         * This destructor ensures that any open database connections are properly closed
         * to prevent resource leaks. It is automatically called when the Database object
         * goes out of scope.
         */
        ~Database();

        /**
         * @brief Initializes the database and creates necessary tables.
         * 
         * This method prepares the database for use by executing SQL commands
         * to create required tables if they do not already exist. It should
         * be called after constructing the Database object to ensure that the
         * database is set up correctly before any operations are performed.
         */
        void init();

        /**
         * @brief Returns the SQLite database connection pointer.
         * 
         * This method provides access to the underlying SQLite database connection.
         * It can be used for executing raw SQL commands directly. 
         * 
         * @return A pointer to the sqlite3 database connection object. 
         * The caller should not attempt to close this connection directly, 
         * as it is managed by the Database class.
         */
        sqlite3* getDBConnection() const;


        /**
         * @brief Inserts a new record into the specified table.
         * 
         * This method constructs and executes an SQL INSERT statement to add a new record
         * to the given table. It uses a field mapping to specify which properties of the
         * provided data object correspond to which columns in the database table.
         * 
         * @tparam T The type of the data object being inserted. The type must have
         *           properties that can be accessed via the provided field mapping.
         * 
         * @param tableName The name of the table into which the data will be inserted.
         * 
         * @param data The data object containing the values to be inserted into the table.
         * 
         * @param fieldMapping A map that associates column names in the table with
         *                     functions that retrieve the corresponding values from the
         *                     data object. The functions should return a variant type
         *                     containing either a string, int, or double.
         * 
         * @return true if the insert operation was successful; false otherwise. 
         * 
         * This method prepares the SQL statement, binds the values to the placeholders,
         * and executes the INSERT operation. If any errors occur during preparation or
         * execution, an error message is printed to standard error.
         */
        template <typename T>
        bool insert(const std::string &tableName, const T &data, const std::map<std::string, std::function<std::variant<std::string, int, double>(const T&)>> &fieldMapping){
            std::string sql = "INSERT INTO "+ tableName + "( ";
            std::string placeholders = " VALUES (";
            for (auto &[columnName, getter] : fieldMapping){
                sql += columnName + ",";
                placeholders += "?, ";
            }

            sql.pop_back();
            placeholders.pop_back();

            sql += placeholders;

            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
                std::cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }

            int index = 1;

            for (auto &[columnName, getter] : fieldMapping){
                std::variant<std::string, int, double> value = getter(data);

                if(std::holds_alternative<int>(value)){
                    sqlite3_bind_int(stmt, index++, std::get<int>(value));
                }else if(std::holds_alternative<double>(value)){
                    sqlite3_bind_double(stmt, index++, std::get<double>(value));
                }else if(std::holds_alternative<std::string>(value)){
                    const std::string &strValue = std::get<std::string>(value);
                    sqlite3_bind_text(stmt, index++, strValue.c_str(), -1, SQLITE_TRANSIENT);
                }
            }

            if (sqlite3_step(stmt) != SQLITE_DONE){
                std::cerr << "Error executing INSERT statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return false;
            }

            sqlite3_finalize(stmt);
            return true;
        }


        /**
         * @brief Updates an existing record in the specified table.
         * 
         * This method constructs and executes an SQL UPDATE statement to modify an existing
         * record in the given table identified by its unique ID. It uses a field mapping
         * to specify which properties of the provided data object correspond to which columns
         * in the database table.
         * 
         * @tparam T The type of the data object being used for the update. The type must
         *           have properties that can be accessed via the provided field mapping.
         * 
         * @param tableName The name of the table where the record will be updated.
         * 
         * @param id The unique identifier of the record to be updated. This ID is used
         *           in the WHERE clause of the SQL statement to specify which record
         *           to modify.
         * 
         * @param data The data object containing the new values for the record.
         * 
         * @param fieldMapping A map that associates column names in the table with
         *                     functions that retrieve the corresponding values from the
         *                     data object. The functions should return a variant type
         *                     containing either a string, int, or double.
         * 
         * @return true if the update operation was successful; false otherwise. 
         * 
         * This method prepares the SQL statement, binds the values to the placeholders,
         * and executes the UPDATE operation. If any errors occur during preparation or
         * execution, an error message is printed to standard error. Additionally, if no
         * rows are affected by the update, a message is logged indicating that the ID may not exist.
         */
        template <typename T>
        bool update(const std::string &tableName, const int &id, const T &data, const std::map<std::string, std::function<std::variant<std::string, int, double>(const T&)>> &fieldMapping){
            std::string sql = "UPDATE "+ tableName +" SET ";
            for (auto &[columnName, getter] : fieldMapping){
                sql += columnName + "?, ";
            }

            sql.pop_back();
            sql.pop_back();
            sql += " WHERE id = ?;";

            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
                std::cerr << "Error preparing UPDATE statement: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }

            int index = 1;

            for(auto &[columnName, getter] : fieldMapping){
                std::variant<std::string, int, double> value = getter(data);
                
                if(std::holds_alternative<int>(value)){
                    sqlite3_bind_int(stmt, index++, std::get<int>(value));
                }else if(std::holds_alternative<double>(value)){
                    sqlite3_bind_double(stmt, index++, std::get<double>(value));
                }else if(std::holds_alternative<std::string>(value)){
                    const std::string &strValue = std::get<std::string>(value);
                    sqlite3_bind_text(stmt, index++, strValue.c_str(), -1, SQLITE_STATIC);
                }
            }

            sqlite3_bind_int(stmt, index++, id);

            if (sqlite3_step(stmt) != SQLITE_DONE){
                std::cerr << "Error executing update statement: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return false;
            }

            int changes = sqlite3_changes(db);
            if (changes == 0) {
                std::cerr << "No rows were updated. Check if the ID exists." << std::endl;
            }

            sqlite3_finalize(stmt);
            return true;
        }

        /**
         * @brief Removes a record from the specified table.
         * 
         * This method constructs and executes an SQL DELETE statement to remove a record
         * from the given table identified by its unique ID.
         * 
         * @param tableName The name of the table from which the record will be deleted.
         * 
         * @param id The unique identifier of the record to be removed. This ID is used
         *           in the WHERE clause of the SQL statement to specify which record
         *           should be deleted.
         * 
         * @return true if the remove operation was successful; false otherwise.
         * 
         * If any errors occur during the execution of the DELETE statement, an error message
         * is printed to standard error. Additionally, if no rows are affected by the delete,
         * a message is logged indicating that the ID may not exist.
         */
        bool remove(const std::string &tableName, const int &id);
};

#endif