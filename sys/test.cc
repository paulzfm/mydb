#include "SystemManager.h"

int main() {
	SystemManager manager;
	manager.createDatabase("dbtest");
	manager.showDatabases();
	manager.useDatabase("dbtest");
	manager.createTable("23333", 5);
	manager.showTables();

	Table& table = manager.createTable("12345", 2);
	Column c = Column(0, "col1", 107, 4, 0);
	table.addColumn(c);
	Column& col = table.getColumn(table.getColumnByName("col1"));
	rapidjson::Document doc;
	rapidjson::Value value(0);

	Constraint cc = Constraint(col.cid, "PK", Constraint::PRIMARY_KEY, value);
	table.addConstraint(cc);
	cc = Constraint(col.cid, "NN", Constraint::NOT_NULL, value);
	table.addConstraint(cc);
	c = Column(0, "col2", 108, 4, 0);
	table.addColumn(c);

	manager.showTables();
	manager.descTable("12345");
	manager.descTable("23333");
	manager.dropTable("23333");
	manager.showTables();
	manager.dropDatabase("dbtest");
	manager.showDatabases();

    cout << cmsg.str();
}
