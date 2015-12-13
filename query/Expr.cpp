#include "Expr.h"
#include <cassert>

Expr::Expr(const Operator& _op, const std::string& _column,
		const vector<Expr*>& _childs) : op(_op), column(_column), childs(_childs) {
	value.len = 0;
}

void Expr::setValue(const Value& val) {
	assert(childs.size() == 0 && column == "");
	this->val = val;
}

Value Expr::getValue(const Table* table, const Record& record) const {
	if (value.len == 0) return value;

	if (op != NullOp) {
		vector<Value> values;
		for (auto& expr : childs)
			values.push_back(expr.getValue(table, record));
		return op.apply(values);
	}

	if (column != "") {
		Column& col = table->getColumnByName(column);
		switch (col.def.type) {
			case DType::BYTE:
				return Value(Byte().unserialize(record.data + col.def.offset));
			case DType::SHORT:
				return Value(Short().unserialize(record.data + col.def.offset));
			case DType::INT:
				return Value(Int().unserialize(record.data + col.def.offset));
			case DType::LONG:
				return Value(Long().unserialize(record.data + col.def.offset));
			case DType::FLOAT:
				return Value(Float().unserialize(record.data + col.def.offset));
			case DType::DOUBLE:
				return Value(Double().unserialize(record.data + col.def.offset));
			case DType::CHAR:
				return Value(Char().unserialize(record.data + col.def.offset));
			case DType::STRING:
				return Value(String().unserialize(record.data + col.def.offset));
			default:
				// should not reach here
				return NullValue();
		}
	}
}

std::function<bool(const Record&)> Expr::getFilter(const Table* table) const {
	return [this, table] (const Record& record) {
		Value val = this->getValue(table, record);
		assert(val.type == DType::BYTE);
		return Byte().unserialize(val.data);
	};
}
