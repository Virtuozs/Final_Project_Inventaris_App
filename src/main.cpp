#include "database.hpp"

int main(){
    Database *db = new Database("inventaris_app.db");

    db->init();

    return 0;
}