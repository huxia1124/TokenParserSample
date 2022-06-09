
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

// This is the utility function that split a string by given separators.
// It supports quoted tokens.
// Examples are in the main function below.

template<typename TString>
bool ParseTokens(const typename TString::value_type* text, const typename TString::value_type* separators, bool removeEmptyTokens, std::vector<typename TString>& outResult)
{
	static_assert(std::is_base_of<std::basic_string<char>, TString>::value || std::is_base_of<std::basic_string<wchar_t>, TString>::value, "TString must be std::string or std::wstring");

	outResult.clear();

	// Empty separator list is not allowed
	if (separators == nullptr || separators[0] == 0)
		return false;

	// Quotation mark can not be used as a separator
	const typename TString::value_type* sep = separators;
	while (*sep)
	{
		if (*sep == '\"')
			return false;

		++sep;
	}

	auto isSeparator = [separators](typename TString::value_type ch)
	{
		const typename TString::value_type* sep = separators;
		while (*sep)
		{
			if (*sep == ch)
			{
				return true;
			}

			++sep;
		}
		return false;
	};

	enum State
	{
		Token = 1,
		Quote = 2,
	};

	State state = Token;
	const typename TString::value_type* p = text;
	const typename TString::value_type* lastPos = p;
	while (true)
	{
		switch (state)
		{
		case Token:
			if (!*p)
			{
				if (p != lastPos || !removeEmptyTokens)
				{
					outResult.emplace_back(lastPos, p);
				}
				return true;
			}
			else if (*p == '\"')
			{
				state = Quote;
			}
			else if (isSeparator(*p))
			{
				if (p != lastPos || !removeEmptyTokens)
				{
					outResult.emplace_back(lastPos, p);
				}
				lastPos = p + 1;
			}
			break;
		case Quote:	// in quote
			if (!*p)
			{
				outResult.clear();
				return false;
			}
			else if (*p == '\"')
			{
				state = Token;
			}
			break;
		}

		if (!*p)
		{
			break;
		}

		++p;
	}

	return true;
}


int main()
{
	std::vector<std::string> result;
	result.push_back("token_old");

	const char* input = "A long sentence with many tokens, even \"quoted tokens\" and something like \"(a,b,c)\".";


	ParseTokens(input, ",. ", true, result);
	for (auto itr = result.begin(); itr != result.end(); ++itr)
	{
		std::cout << *itr << std::endl;
	}

	/* ---- output begin ----
	A
	long
	sentence
	with
	many
	tokens
	even
	"quoted tokens"
	and
	something
	like
	"(a,b,c)"
	---- output end ---- */

	assert(!ParseTokens("test", nullptr, true, result));		// Invalid separators
	assert(result.empty());

	assert(!ParseTokens("test", "", true, result));		// Invalid separators
	assert(result.empty());

	assert(!ParseTokens("test", "\"", true, result));		// Invalid separators
	assert(result.empty());

	assert(ParseTokens("", " ", true, result));
	assert(result.empty());

	assert(ParseTokens("test", " ", true, result));
	assert(result.size() == 1);
	assert(result[0] == "test");

	assert(ParseTokens("test sample", " ", true, result));
	assert(result.size() == 2);
	assert(result[0] == "test");
	assert(result[1] == "sample");

	assert(ParseTokens("test sample", " ", true, result));
	assert(result.size() == 2);
	assert(result[0] == "test");
	assert(result[1] == "sample");

	assert(ParseTokens("  test sample   ", " ", true, result));
	assert(result.size() == 2);
	assert(result[0] == "test");
	assert(result[1] == "sample");

	assert(ParseTokens(" test sample  ", " ", false, result));
	assert(result.size() == 5);
	assert(result[0] == "");
	assert(result[1] == "test");
	assert(result[2] == "sample");
	assert(result[3] == "");
	assert(result[4] == "");

	assert(ParseTokens(" test sample \"quoted\"test", " ", true, result));
	assert(result.size() == 3);
	assert(result[0] == "test");
	assert(result[1] == "sample");
	assert(result[2] == "\"quoted\"test");

	assert(ParseTokens(" test sample   \"quoted text\"", " ", true, result));
	assert(result.size() == 3);
	assert(result[0] == "test");
	assert(result[1] == "sample");
	assert(result[2] == "\"quoted text\"");

	assert(ParseTokens(" a \"test sample\"   \"quoted text\"", " ", true, result));
	assert(result.size() == 3);
	assert(result[0] == "a");
	assert(result[1] == "\"test sample\"");
	assert(result[2] == "\"quoted text\"");

	assert(ParseTokens("\"\"", " ", true, result));
	assert(result.size() == 1);
	assert(result[0] == "\"\"");

	assert(ParseTokens("   \"\"", " ", true, result));
	assert(result.size() == 1);
	assert(result[0] == "\"\"");

	assert(ParseTokens("\"\"   ", " ", true, result));
	assert(result.size() == 1);
	assert(result[0] == "\"\"");

	assert(ParseTokens("    \"\"   ", " ", true, result));
	assert(result.size() == 1);
	assert(result[0] == "\"\"");

	assert(ParseTokens("    \"\"  \"\"   ", " ", true, result));
	assert(result.size() == 2);
	assert(result[0] == "\"\"");
	assert(result[1] == "\"\"");


	assert(!ParseTokens("    \"\"  \"   ", " ", true, result));		// quotes not closed
	assert(result.empty());
	assert(!ParseTokens("    \"   ", " ", true, result));		// quotes not closed
	assert(result.empty());


	assert(ParseTokens("  a;\"test;sample\";  \"quoted text\"", ";", true, result));
	assert(result.size() == 3);
	assert(result[0] == "  a");
	assert(result[1] == "\"test;sample\"");
	assert(result[2] == "  \"quoted text\"");

	assert(ParseTokens("  a;\"test;sample\";  \"quoted text\"", "; ", true, result));		// two separators
	assert(result.size() == 3);
	assert(result[0] == "a");
	assert(result[1] == "\"test;sample\"");
	assert(result[2] == "\"quoted text\"");

	// Unicode version
	std::vector<std::wstring> wresult;
	assert(ParseTokens(L" is a;\"test;sample\";  \"quoted text\"", L"; ", true, wresult));
	assert(wresult.size() == 4);
	assert(wresult[0] == L"is");
	assert(wresult[1] == L"a");
	assert(wresult[2] == L"\"test;sample\"");
	assert(wresult[3] == L"\"quoted text\"");
}