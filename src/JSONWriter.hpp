#pragma once

#include <ostream>
#include <stack>

class JSONWriter {
public:
	JSONWriter(std::ostream& stream): m_stream(stream) { }

	void begin();
	void beginArray();
	void end();

	void writeString(const std::string& str);
	void writeInt(int value);
	void writeUint(unsigned int value);
	void writeBool(bool value);
	void writeDouble(double value);

	void key(const std::string& str);

private:
	struct Context {
		enum {
			TypeObject,
			TypeArray,
			TypeKey
		};

		unsigned int m_type;
		bool m_needsComma;
	};

	void prefix();
	void indent();

	std::ostream& m_stream;
	std::stack<Context> m_context;
};
