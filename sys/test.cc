#include "SystemManager.h"

int main() {
	SystemManager manager;
	manager.createDatabase("dbtest");
	manager.showDatabases();
	manager.useDatabase("dbtest");
	manager.createTable("23333");
	manager.showTables();
	Table& table = manager.createTable("12345");
	table.addColumn(Column(ColumnDef{0, "col1", 107}));
	Column& col = table.getColumnByName("col1");
	col.addConstraint(ColumnConstraint{col.def.cid, 0, 0});
	col.addConstraint(ColumnConstraint{col.def.cid, 1, 0});
	table.addColumn(Column(ColumnDef{0, "col2", 108}));
	manager.showTables();
	manager.descTable("12345");
	manager.descTable("23333");
	manager.dropTable("23333");
	manager.showTables();
	manager.dropDatabase("dbtest");
	manager.showDatabases();
}
