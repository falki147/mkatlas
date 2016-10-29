#include "JSONWriter.hpp"

static void writeString(std::ostream& stream, const std::string& value) {
	const char* table[] = {
		"\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
		"\\b",     "\\t",     "\\n",     "\\u0011", "\\f",     "\\r",     "\\u0014", "\\u0015",
		"\\u0016", "\\u0017", "\\u0018", "\\u0019", "\\u0020", "\\u0021", "\\u0022", "\\u0023",
		"\\u0024", "\\u0025", "\\u0026", "\\u0027", "\\u0028", "\\u0029", "\\u0030", "\\u0031",
	};

	stream << '"';

	auto last = value.begin();

	for (auto it = value.begin(); it != value.end(); ++it) {
		if (*it == '"') {
			stream.write(&*last, it - last);
			stream << "\\\"";
			last = std::next(it);
		}
		else if (*it == '\\') {
			stream.write(&*last, it - last);
			stream << "\\\\";
			last = std::next(it);
		}
		else if (*it < 0x20) {
			stream.write(&*last, it - last);
			stream << table[*it];
			last = std::next(it);
		}
	}

	if (last != value.end())
		stream.write(&*last, value.end() - last);

	stream << '"';
}

void JSONWriter::prefix() {
	if (!m_context.empty()) {
		if (m_context.top().m_type == Context::TypeKey) {
			m_context.pop();
			return;
		}

		if (m_context.top().m_needsComma)
			m_stream << ",\n";
		else
			m_context.top().m_needsComma = true;
	}

	indent();
}

void JSONWriter::indent() {
	for (unsigned int i = 0; i < m_context.size(); ++i)
		m_stream << '\t';
}

void JSONWriter::begin() {
	prefix();
	m_stream << "{\n";
	m_context.push({ Context::TypeObject, false });
}

void JSONWriter::beginArray() {
	prefix();
	m_stream << "[\n";
	m_context.push({ Context::TypeArray, false });
}

void JSONWriter::end() {
	auto con = m_context.top();
	m_context.pop();

	m_stream << '\n';
	indent();

	switch (con.m_type) {
	case Context::TypeObject:
		m_stream << "}";
		break;
	case Context::TypeArray:
		m_stream << "]";
		break;
	}
}

void JSONWriter::writeString(const std::string& str) {
	prefix();
	::writeString(m_stream, str);
}

void JSONWriter::writeInt(int value) {
	prefix();
	m_stream << value;
}

void JSONWriter::writeUint(unsigned int value) {
	prefix();
	m_stream << value;
}

void JSONWriter::writeBool(bool value) {
	prefix();
	m_stream << (value ? "true" : "false");
}

void JSONWriter::writeDouble(double value) {
	prefix();
	m_stream << value;
}

void JSONWriter::key(const std::string& str) {
	prefix();
	::writeString(m_stream, str);
	m_stream << ": ";
	m_context.push({ Context::TypeKey, false });
}
