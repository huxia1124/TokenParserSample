# TokenParserSample
This is a C++ utility function that splits a string by given separators.
It supports quoted tokens.
Unicode/Wide characters are also supported

Usage:
```c++
	std::vector<std::string> result;
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
```
