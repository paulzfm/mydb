#include "Column.h"

Column NullColumn = Column(ColumnDef{-1, "Null Column", 0});

Column::Column() {
}

Column::Column(const ColumnDef &def) {
	this->def = def;
}

bool Column::addConstraint(ColumnConstraint c) {
	if (c.cid != def.cid) return false;
	constraints.push_back(c);
	return true;
}

bool Column::check(const char* data) const {
	return true;
}
