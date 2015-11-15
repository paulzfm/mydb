#include "SystemManager.h"

int main() {
	SystemManager manager;
	manager.createDatabase("dbtest");
	manager.showDatabases();
	manager.useDatabase("dbtest");
	manager.createTable("23333");
	manager.showTables();
	manager.createTable("12345");
	manager.showTables();
	manager.dropTable("23333");
	manager.showTables();
	manager.dropDatabase("dbtest");
	manager.showDatabases();
}
