#include "Column.h"

Column::Column() {
}

Column::Column(ColumnDef &def) {
	this->def = def;
}

bool Column::addConstraint(ColumnConstraint c) {
	if (c.cid != def.cid) return false;
	constraints.push_back(c);
}

bool Column::check(const char* data) const {
}
